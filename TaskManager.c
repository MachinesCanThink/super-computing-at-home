/* A Client in the Internet domain using TCP. 
 * Sends a Python script to the server and fetches results back.
 * To be passed name of the server followed by the port number to run and the python script.
 */

#include <stdio.h>
#include <sys/types.h>	// data types used in syscalls.
#include <sys/socket.h>	// structures needed for sockets.
#include <netinet/in.h> // structures needed for IP addresses.
#include <string.h>
#include <stdlib.h>
#include <netdb.h>	// for DNS lookup.
#include <errno.h>
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
 * Sets up the Client using sockets 
 */
void setupClient(int argc, char* argv[], struct hostent* server, int port_no, int* socket_fd, struct sockaddr_in* server_addr) 
{
	// Create socket using socket() 
	*socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (*socket_fd < 0)
		error("Client: error, socket() call failed");

	/* 
	 * Set the fields in server address. 
	 */
	bzero((char*) server_addr, sizeof(*server_addr));	// fill zeros.
	(*server_addr).sin_family = AF_INET;
	bcopy((char*) server -> h_addr, (char*) &(*server_addr).sin_addr.s_addr, server -> h_length);
	(*server_addr).sin_port = htons(port_no);	// convert to network byte ordering.

	/* 
	 * After server address has been defined, connect() 
	 */
	if (connect(*socket_fd, (struct sockaddr*) &(*server_addr), sizeof(*server_addr)))
		error("Client: error connecting\n");
}

/* 
 * Sends Python script to server socket 
 */
void sendPythonScript(char* buffer, int socket_fd) 
{
	printf("Client: Sending Python script.\n");
	int n = write(socket_fd, buffer, 255);
	printf("Client: Sent. Waiting for results\n");
	if (n < 0)
		error("Client: error writing to socket\n");

	printf("Server: result of script:\n");
	n = read(socket_fd, buffer, 255);
	if (n < 0){
		printf("Client: error reading from socket\n");
		printf("errorno = %s\n", strerror(errno));
	}
	printf ("%s\n", buffer);
}

/* 
 * Task manager main 
 */
int main (int argc, char* argv[]) 
{	
	/* 
	 * Initial setting up 
	 */
	int socket_fd;	// socket file descriptor.
	int port_no;	// port number where the server accepts communication.
	int n;	// return value of read() and write().
	struct sockaddr_in server_addr;	// server IP.
	struct hostent* server;	// defines a host on the Internet.
	char buffer[256];	// buffer to hold Python script.
	int python_fd;	// python script's file descriptor.

	/* 
	 * Flag error if server name or port number or the Python script not specified. 
	 */
	if (argc < 4) {
		fprintf(stderr, "Client: pass name of the server followed by the port number followed by Python script\n");
		exit(1);
	}

	server = gethostbyname(argv[1]);	// do a DNS lookup of server name.
	if (server == NULL) {
		fprintf(stderr, "Client: error, no such host\n");
		exit(1);
	}

	port_no = atoi(argv[2]);	// port number passed ASCII to int.
	python_fd   = open(argv[3], O_RDONLY);	// opening python script.
	
	n = read(python_fd, buffer, 255);
	buffer[n] = '\0'; 
	if (n < 0){
		printf("Client: error reading pyscript\n");
		exit(1);
	}
	/* 
	 * Initial setting up end 
	 */

	setupClient(argc, argv, server, port_no, &socket_fd, &server_addr);
	
	sendPythonScript(buffer, socket_fd);

	return 0;
}