/* A simple server in the Internet domain using TCP that interprets a python script.
 * The port number is passed as argument.
 */

#include <stdio.h>
#include <sys/types.h>	// data types used in syscalls.
#include <sys/socket.h>	// structures needed for sockets.
#include <netinet/in.h> // structures needed for IP addresses.
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>

/* error() is called when a syscall fails.
 * Writes the error message in stderr and then aborts.
 */ 
void error(char* msg)
{
	perror(msg);
	exit(1);
}


/* Setup server code */
void setupServer(int portNo, int* sockFd, int* newSockFd, struct sockaddr_in* servAddr, struct sockaddr_in* cliAddr, int cliAddrLength) {
	*sockFd = socket(AF_INET, SOCK_STREAM, 0); // create server socket
	if (*sockFd < 0)	
		error("server: error opening socket\n");	// flag error is socket() fails
	/* AF_INET represents a socket in the Internet domain.
	 * SOCK_STREAM represents a stream socket based on TCP.
	 * 0 is protocol that's automatically chosen by OS. TCP in this case.
	 */

	bzero((char*)&(*servAddr), sizeof((*servAddr)));	// sets all values in (*servAddr) to zero

	(*servAddr).sin_family 	    = AF_INET;	// set addr type to IP.
	(*servAddr).sin_port 		= htons(portNo);	// convert into network byte order.
	(*servAddr).sin_addr.s_addr = INADDR_ANY;	// IP address of the server.

	if (bind(*sockFd, (struct sockaddr*) &(*servAddr), sizeof((*servAddr))) < 0)
		error("server: error on binding");	
	/* The bind() syscall binds a socket to an address,
	 * in this case the address of the current host and port number on which the server will run. 
	 * It takes three arguments:
	 * the socket file descriptor, the address to which is bound, and the size of the address to which it is bound.
	 */

	listen(*sockFd, 5);	// allows the process to listen on the socket for connections.

	cliAddrLength = sizeof((*cliAddr));

	/* The accept() system call causes the process to block until a client connects to the server. 
	 * Thus, it wakes up the process when a connection from a client has been successfully established.
	 */
	*newSockFd = accept(*sockFd, (struct sockaddr*) &(*cliAddr), &cliAddrLength);
	if (*newSockFd < 0)
		error("server: error on accept");
}

/* Interprets recieved python script by calling system().
 * Writes output to a file.
 */
void interpretPython(int pyFd) {
	int pyResultFd = open("PyResult", O_WRONLY);
	char command[] = "python PyScript.py > PyResult";
	system(command);
	close(pyResultFd);
}

/* PyResult is sent back. */
void sendResultBack(int newSockFd) {
	char buffer[256];
	int pyResultFd = open("PyResult", O_RDONLY);
	int n = read(pyResultFd, buffer, 255);
	if (n < 0){
		fprintf(stderr, "Server: error reading\n");
		exit(1);
	}
	buffer[n] = '\0';

	/* Write to socket */
	n = write(newSockFd, buffer, 255);
}

/* Slave main */
int main(int argc, char* argv[])
{
    int sockFd, newSockFd;	// socket file descriptors into the file descriptor table returned by socket().
	int portNo;	// port number on which the server accepts communication.
	int cliAddrLength;	// address length of client.
	int n;	// return value from read() or write().
	char buffer[256];	// server reads characters from the socket into this buffer.
	int pyFd;
	struct sockaddr_in servAddr, cliAddr;	// structures to hold IP.

	if (argc < 2)	// flag error if port number is provided.
	{
		fprintf(stderr, "server: error, no port provided as command line arg\n");
		exit(1);
	}

	portNo = atoi(argv[1]);	// passed port number from ASCII to int.

	setupServer(portNo, &sockFd, &newSockFd, &servAddr, &cliAddr, cliAddrLength);

	/* Read from client established by accepting on newSockFd. 
	 * Print the message on stdout.
	 */
	n = read(newSockFd, buffer, 255);
	if (n < 0)
		error("server: error reading from socket");


	/* Read python script from buffer to file. */
	pyFd = open("PyScript.py", O_WRONLY);
	n = write(pyFd, buffer, 255);
	if (n < 0) {
		fprintf(stderr, "Server: write error.\n");
		exit(1);
	}

	interpretPython(pyFd);

	sendResultBack(newSockFd);

    return 0; 
    /* end server */
}