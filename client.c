/* A simple client in the Internet domain using TCP. 
 * To be passed name of the server followed by the port number to run.
 */

#include <stdio.h>
#include <sys/types.h>	// data types used in syscalls.
#include <sys/socket.h>	// structures needed for sockets.
#include <netinet/in.h> // structures needed for IP addresses.
#include <string.h>
#include <stdlib.h>
#include <netdb.h>
#include <errno.h>

/* error() is called when a syscall fails.
 * Writes the error message in stderr and then aborts.
 */ 
void error(char* msg)
{
	perror(msg);
	exit(1);
}

/* Client main */
int main (int argc, char* argv[])
{
	int sockFd;	// socket file descriptor.
	int portNo;	// port number where the server accepts communication.
	int n;	// return value of read() and write().
	struct sockaddr_in servAddr;	// server IP.
	struct hostent* server;	// defines a host on the Internet.
	char buffer[256];

	/* Flag error if server name and port number not specified. */
	if (argc < 3)
	{
		fprintf(stderr, "client: pass name of the server followed by the port number\n");
		exit(1);
	}

	portNo = atoi(argv[2]);	// port number passed ASCII to int.

	/* Create socket using socket() */
	sockFd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockFd < 0)
		error("client: error, socket() call failed");

	/* struct hostent* getnamebyhost(char*) */
	server = gethostbyname(argv[1]);
	if (server == NULL)
	{
		fprintf(stderr, "client: error, no such host\n");
		exit(1);
	}

	/* Set the fields in server address. */
	bzero((char*) &servAddr, sizeof(servAddr));	// fill zeros.
	servAddr.sin_family = AF_INET;
	bcopy((char*) server -> h_addr, (char*) &servAddr.sin_addr.s_addr, server -> h_length);
	servAddr.sin_port = htons(portNo);	// convert to network byte ordering.

	/* After server address has been defined, connect() */
	if (connect(sockFd, (struct sockaddr*) &servAddr, sizeof(servAddr)))
		error("client: error connecting\n");

	while(1)	// for continuous connection.
	{
		printf("Please enter the message to be sent\n");
		bzero(buffer, 256);
		fgets(buffer, 255, stdin);
		n = write(sockFd, buffer, 255);
		if (n < 0)
			error("client: error writing to socket\n");

		n = read(sockFd, buffer, 255);
		if (n < 0)
		{
			printf("client: error reading from socket\n");
			printf("errorno = %s\n", strerror(errno));
		}
		printf ("%s\n", buffer);
	}	
	return 0;
	/* end client */
}