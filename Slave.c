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

/* 
 * error() is called when a syscall fails.
 * Writes the error message in stderr and then aborts.
 */ 
void error(char* msg)
{
	perror(msg);
	exit(1);
}

/* 
 * Setup server code 
 */
void setupServer(int port_no, int* socket_fd, int* new_socket_fd, struct sockaddr_in* server_addr, struct sockaddr_in* client_addr, int client_addr_length) 
{
	*socket_fd = socket(AF_INET, SOCK_STREAM, 0); // create server socket
	if (*socket_fd < 0)	
		error("server: error opening socket\n");	// flag error is socket() fails
	/* 
	 * AF_INET represents a socket in the Internet domain.
	 * SOCK_STREAM represents a stream socket based on TCP.
	 * 0 is protocol that's automatically chosen by OS. TCP in this case.
	 */

	bzero((char*)&(*server_addr), sizeof((*server_addr)));	// sets all values in (*servAddr) to zero

	(*server_addr).sin_family 	    = AF_INET;	// set addr type to IP.
	(*server_addr).sin_port 		= htons(port_no);	// convert into network byte order.
	(*server_addr).sin_addr.s_addr = INADDR_ANY;	// IP address of the server.

	if (bind(*socket_fd, (struct sockaddr*) &(*server_addr), sizeof((*server_addr))) < 0)
		error("server: error on binding");	
	/* 
	 * The bind() syscall binds a socket to an address,
	 * in this case the address of the current host and port number on which the server will run. 
	 * It takes three arguments:
	 * the socket file descriptor, the address to which is bound, and the size of the address to which it is bound.
	 */

	listen(*socket_fd, 5);	// allows the process to listen on the socket for connections.

	client_addr_length = sizeof((*client_addr));

	/* 
	 * The accept() system call causes the process to block until a client connects to the server. 
	 * Thus, it wakes up the process when a connection from a client has been successfully established.
	 */
	*new_socket_fd = accept(*socket_fd, (struct sockaddr*) &(*client_addr), &client_addr_length);
	if (*new_socket_fd < 0)
		error("server: error on accept");
}

/* 
 * Interprets recieved python script by calling system().
 * Writes output to a file.
 */
void interpretPython(int python_fd) 
{
	int python_result_fd = open("PyResult", O_WRONLY);
	char command[] = "python PyScript.py > PyResult";
	system(command);
	close(python_result_fd);
}

/*
 * PyResult is sent back.
 */ 
void sendResultBack(int new_socket_fd) 
{
	char buffer[256];
	int python_result_fd = open("PyResult", O_RDONLY);
	int n = read(python_result_fd, buffer, 255);
	if (n < 0) {
		fprintf(stderr, "Server: error reading\n");
		exit(1);
	}
	buffer[n] = '\0';

	// Write to socket 
	n = write(new_socket_fd, buffer, 255);
}

/* 
 * Slave main 
 */
int main(int argc, char* argv[])
{
    int socket_fd;	// socket file descriptors into the file descriptor table returned by socket().
    int new_socket_fd;	// socket file descriptors into the file descriptor table returned by socket().
	int port_no;	// port number on which the server accepts communication.
	int client_addr_length;	// address length of client.
	int n;	// return value from read() or write().
	char buffer[256];	// server reads characters from the socket into this buffer.
	int python_fd;
	struct sockaddr_in server_addr;	// structures to hold IP.
	struct sockaddr_in client_addr;	// structures to hold IP.

	if (argc < 2) {	
		// flag error if port number is provided.
		fprintf(stderr, "server: error, no port provided as command line arg\n");
		exit(1);
	}

	port_no = atoi(argv[1]);	// passed port number from ASCII to int.

	setupServer(port_no, &socket_fd, &new_socket_fd, &server_addr, &client_addr, client_addr_length);

	/* 
	 * Read from client established by accepting on newSockFd. 
	 * Print the message on stdout.
	 */
	n = read(new_socket_fd, buffer, 255);
	if (n < 0)
		error("server: error reading from socket");


	/* 
	 * Read python script from buffer to file. 
	 */
	python_fd = open("PyScript.py", O_WRONLY);
	n = write(python_fd, buffer, 255);
	if (n < 0) {
		fprintf(stderr, "Server: write error.\n");
		exit(1);
	}

	interpretPython(python_fd);

	sendResultBack(new_socket_fd);

    return 0; 
    // end server 
}