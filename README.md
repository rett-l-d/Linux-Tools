# Char Driver
* Linux Char Device Driver for Kernel 6.x +.
* Used the book LDD and other more modern references to build it.
* Use OPEN, READ and WRITE functions to interact with it.
* Uses the kernel buffer, dynamically allocated, to store the information and retreive it.

# C Sockets Server
* Multi-threaded TCP sockets server, using linked lists to manage ultiple client connections. 
* The server receives client data packets and writes them into a file in the /tmp folder.
* The server uses a mutex lock to prevent overwriting data from parallel requests.

# Get CSV 
* Bash tool to connect to a remote embedded controller, the user can select serial or TCP sockets for the connection.
* The user can choose between downloading the data from a CSV file to the controller or upload from it to a new file.
* The data formatting in this tool was for a specific application.
* To re-use it adjust to the data structure required.

# Python Sockets Client 
* This tool was developed to connect with an embedded controller through sockets while at the same time executing a robotic simulation with RoboDK.
* It's a multi-thread tool that receives 60 targets, buffering them while another thread executes the motions instructions, so there is not delay between receiving the data points and executing the motions.
* The targets are structured as 6D poses and before the end of the buffer, the task requests another 60 data points and so on until the controller ends the trasnmission.
