

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/slab.h>      /* kmalloc, kfree */
#include <linux/uaccess.h>   /* copy_to_user, copy_from_user */

#define MEM_SIZE 1024

dev_t dev = 0;
static struct class *dev_class;
static struct cdev chr_cdev;
uint8_t *kernel_buffer;

struct mydev {
     struct cdev cdev; 
     int data;
 };

/* Function prototypes */
static int chr_open(struct inode *inode, struct file *file);
static int chr_release(struct inode *inode, struct file *file);
static ssize_t chr_read(struct file *filp, char __user *buf, size_t len, loff_t *off);
static ssize_t chr_write(struct file *filp, const char __user *buf, size_t len, loff_t *off);
static int __init chr_driver_init(void);
static void __exit chr_driver_exit(void);

/* File operations structure */
static struct file_operations fops = {
    .owner   = THIS_MODULE,
    .read    = chr_read,
    .write   = chr_write,
    .open    = chr_open,
    .release = chr_release,
};

/* Open function */
static int chr_open(struct inode *inode, struct file *file)
{
    struct mydev *dev;
   // Convert inode->i_cdev back to our mydev struct
    dev = container_of(inode->i_cdev, struct mydev, cdev);

    // Store for other operations (read/write)
    file->private_data = dev;

    printk(KERN_INFO "mydev: device opened\n");
    return 0;
}

/* Release function */
static int chr_release(struct inode *inode, struct file *file)
{
   
    return 0;
}

/* Read function */
static ssize_t chr_read(struct file *filp, char __user *buf, size_t len, loff_t *off)
{
    /* Copy data from kernel space to user space */
    if (copy_to_user(buf, kernel_buffer, min(len, (size_t)MEM_SIZE))==0) {
        /* return number of bytes copied (here: min(len, MEM_SIZE)) */
        return min(len, (size_t)MEM_SIZE);
       
    }

    printk(KERN_ERR "Failed to copy data to user space\n");
    return -EFAULT; /* Return an error code */
  
}

/* Write function */
static ssize_t chr_write(struct file *filp, const char __user *buf, size_t len, loff_t *off)
{
    /* Copy data from user space to kernel space */
    if (copy_from_user(kernel_buffer, buf, min(len, (size_t)MEM_SIZE)) == 0) {
        printk(KERN_INFO "Write Function\n");
        return min(len, (size_t)MEM_SIZE);
        
    }

    printk(KERN_ERR "Failed to copy data from user space\n");
    return -EFAULT; /* Return an error code */
   
}

/* Initialization function */
static int __init chr_driver_init(void)
{
    /* Allocating Major number */
    if ((alloc_chrdev_region(&dev, 0, 1, "chr_Dev")) < 0) {
        printk(KERN_INFO "Cannot allocate major number\n");
        return -1;
    }

    /* Initialize cdev structure */
    cdev_init(&chr_cdev, &fops);

    /* Adding character device to the system */
    if ((cdev_add(&chr_cdev, dev, 1)) < 0) {
        printk(KERN_INFO "Cannot add the device to the system\n");
        goto r_class;
    }

    /* Creating struct class */
    if ((dev_class = class_create(THIS_MODULE, "chr_class")) == NULL) {
        printk(KERN_INFO "Cannot create the struct class\n");
        goto r_class;
    }

    /* Creating device */
    if ((device_create(dev_class, NULL, dev, NULL, "chr_device")) == NULL) {
        printk(KERN_INFO "Cannot create the Device\n");
        goto r_device;
    }

    printk(KERN_INFO "Device Driver Insert...Done!!!\n");
    return 0;

r_device:
    class_destroy(dev_class);
r_class:
    unregister_chrdev_region(dev, 1);
    cdev_del(&chr_cdev);
    return -1;
}

/* Exit function */
static void __exit chr_driver_exit(void)
{
    device_destroy(dev_class, dev);
    class_destroy(dev_class);
    cdev_del(&chr_cdev);
    unregister_chrdev_region(dev, 1);
    printk(KERN_INFO "Device Driver Remove...Done!!!\n");
}

/* Module initialization and exit macros */
module_init(chr_driver_init);
module_exit(chr_driver_exit);

/* Module information */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("L. Rettore");
MODULE_DESCRIPTION("Linux Character Device Driver");