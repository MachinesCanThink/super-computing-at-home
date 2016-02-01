/* A simple server in the Internet domain using TCP.
 * The port number is passed as argument.
 */

#include <stdio.h>
#include <sys/types.h>	// data types used in syscalls.
#include <sys/socket.h>	// structures needed for sockets.
#include <netinet/in.h> // structures needed for IP addresses.
#include <string.h>
#include <stdlib.h>

/* error() is called when a syscall fails.
 * Writes the error message in stderr and then aborts.
 */ 
void error(char* msg)
{
	perror(msg);
	exit(1);
}

/* Server main */
int main(int argc, char* argv[])
{
    int sockFd, newSockFd;	// socket file descriptors into the file descriptor table returned by socket().
	int portNo;	// port number on which the server accepts communication.
	int cliAddrLength;	// address length of client.
	int n;	// return value from read() or write().
	char buffer[256];	// server reads characters from the socket into this buffer.
	
	struct sockaddr_in servAddr, cliAddr;	// structures to hold IP.

	if (argc < 2)	// flag error if port number is provided.
	{
		fprintf(stderr, "server: error, no port provided as command line arg\n");
		exit(1);
	}

	sockFd = socket(AF_INET, SOCK_STREAM, 0); // create server socket
	if (sockFd < 0)	
		error("server: error opening socket\n");	// flag error is socket() fails
	/* AF_INET represents a socket in the Internet domain.
	 * SOCK_STREAM represents a stream socket based on TCP.
	 * 0 is protocol that's automatically chosen by OS. TCP in this case.
	 */

	bzero((char*)&servAddr, sizeof(servAddr));	// sets all values in servAddr to zero

	portNo = atoi(argv[1]);	// passed port number from ASCII to int.

	servAddr.sin_family 	 = AF_INET;	// set addr type to IP.
	servAddr.sin_port 		 = htons(portNo);	// convert into network byte order.
	servAddr.sin_addr.s_addr = INADDR_ANY;	// IP address of the server.

	if (bind(sockFd, (struct sockaddr*) &servAddr, sizeof(servAddr)) < 0)
		error("server: error on binding");	
	/* The bind() syscall binds a socket to an address,
	 * in this case the address of the current host and port number on which the server will run. 
	 * It takes three arguments:
	 * the socket file descriptor, the address to which is bound, and the size of the address to which it is bound.
	 */

	listen(sockFd, 5);	// allows the process to listen on the socket for connections.

	cliAddrLength = sizeof(cliAddr);

	// int i = 0;
	while(1)
	{
		/* The accept() system call causes the process to block until a client connects to the server. 
		 * Thus, it wakes up the process when a connection from a client has been successfully established.
		 */
		newSockFd = accept(sockFd, (struct sockaddr*) &cliAddr, &cliAddrLength);
		if (newSockFd < 0)
			error("server: error on accept");


		bzero(buffer, 256);
		/* Read from client established by accepting on newSockFd. 
		 * Print the message on stdout.
		 */
		n = read(newSockFd, buffer, 255);
		if (n < 0)
			error("server: error reading from socket");

		printf("Message from client: %s\n", buffer);

		/* Now send a message back to client for ACK. */
		strcpy(buffer, "ACK");

		n = write(newSockFd, buffer, sizeof(buffer));
		if (n < 0)
	    	error("server: error writing to socket");
		
		// i++;
	}
    return 0; 
    /* end server */
}