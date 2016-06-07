#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <syslog.h>	

#define PORT 44124
#define FALLBACK_PORT 44223

using namespace std;

int readFromMaster(int, char*);
int writeToMaster(int, char*);
int createConnectionWithMaster(int&, int);

int createConnectionWithMasterForResults(int socket_fd, string address_non_compatible)
{
    struct sockaddr_in slave_address;

    struct hostent *server;

    int port_number;
    int socket_return_value;

    char *address;

    cout <<"Enter the port number for sending results" <<endl;
    cin >>port_number;

    // Converts C++ string to C string.
    address = new char[address_non_compatible.length() + 1];
    strcpy(address, address_non_compatible.c_str());

    server = gethostbyname(address);

    if (server == NULL) {
        cout <<"ERROR: No such host : " <<address <<endl;
        
        return -1;
    }

    bzero((char *) &slave_address, sizeof(slave_address));

    slave_address.sin_family = AF_INET;

    bcopy((char *)server->h_addr, (char *) &slave_address.sin_addr.s_addr, server->h_length);

    slave_address.sin_port = htons(port_number);

    if (connect(socket_fd, (struct sockaddr*) &slave_address, sizeof(slave_address)) < 0) {
        cout <<"ERROR : connect failed for slave : " <<address <<endl;

        return -1;
    }

    return 1;
}

int createConnectionWithMaster(int &socket_fd, int port_number)
{
	int new_socket_fd;
	int socket_status;

	socklen_t master_length;

	struct sockaddr_in master_address, slave_address;

	if((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		syslog(LOG_NOTICE, "Error creating socket");

		return -1;
	} else {
		syslog(LOG_NOTICE, "Socket created.");
	}

	bzero((char *) &slave_address, sizeof(slave_address));

	slave_address.sin_family = AF_INET;
	slave_address.sin_addr.s_addr = INADDR_ANY;
	slave_address.sin_port = htons(PORT);

	if ((bind(socket_fd, (struct sockaddr*) &slave_address, sizeof(slave_address))) < 0) {
		syslog(LOG_NOTICE, "Error on binding.");

		return -1;
	} else {
		syslog(LOG_NOTICE, "bind happened.");
	}

	listen(socket_fd, 5);

	master_length = sizeof(master_address);

	// Now we can taak. 
	new_socket_fd = accept(socket_fd, (struct sockaddr*) &master_address, &master_length);
    
    if (new_socket_fd < 0) {
        syslog(LOG_NOTICE, "connect failed");

        return -1;
    } else {
    	syslog(LOG_NOTICE, "connected with the master.");

    	return new_socket_fd;
    }

    //close(socket_fd);
    //close(new_socket_fd);

	return 1;
}

int readFromMaster(int socket_fd, char *message)
{
	int socket_status;

	bzero(message, 64);

    if ((socket_status = read(socket_fd, message, 64)) < 0) {
    	syslog(LOG_NOTICE, "Error on read system call");

    	return -1;
    } else {
    	syslog(LOG_NOTICE, "read success: message recieved from master");
    	syslog(LOG_NOTICE, "%s", message);

    	return 1;
    }
}

int writeToMaster(int socket_fd, char *response_message)
{
	int socket_status;

	

	socket_status = write(socket_fd, response_message, 64);

    if (socket_status < 0) {
    	syslog(LOG_NOTICE, "Error on write system call");
    	
   		return -1;
   	} else {
   		syslog(LOG_NOTICE, "write success: message sent");
   	}	
}