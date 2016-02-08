



Purpose
    To implement the socket functions for an HTTP web server by using UNIX Socket.
    
Compile and execute the program
	(1) make clean
		make
	(2) Then follow the "testing_Howto" file step.

What my program does?
	The program basically implement the following functions:
	(1)void init(int port)
	   //This function play a role of initialization for server side,we use "socket","bind",
	   and "listen" to create such function
	(2)int accept_connection(void)
	   //This function is to accepet connections from clients. Use "accept" when 
           implement it.
	(3)int get_request(int fd, char *filename)
	   //This function deals with the message sending from client side.
	(4)int return_result(int fd, char *content type, char *buf, int numbytes)
	(5)int return_error(int fd, char *buf)
	   //These two function return results/error to the client!

Assumptions
   Port number can be specified (1025 - 65535 by default)
   The maxium number of dispatcher/worker will be 100
   The maxium length of the request queue will be 100 requests
  


Error Handling
   Included proper error handling in our implementation.
