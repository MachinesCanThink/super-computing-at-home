#include <iostream>
#include <fstream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <unistd.h>
#include <cstdlib>
#include <string>
#include <cstring>

using namespace std;

int writeToSlave(int, char*);
int readFromSlave(int, char*);
int createConnectionWithSlave(int, string);
void requestSlave(string, char[], char[]);

int createResultSocketConnection(int &socket_fd, int port_number)
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


int writeToSlave(int socket_fd, char* message)
{
	int socket_return_value;

	socket_return_value = write(socket_fd, message, 64);

    if (socket_return_value < 0) {
    	cout <<"ERROR : write failed on socket for slave." <<endl;

    	return -1;
    }

    return 1;
}

int readFromSlave(int socket_fd, char *response_message)
{
	int socket_return_value;

	socket_return_value = read(socket_fd, response_message, 64);

    if (socket_return_value < 0) {
    	cout <<"ERROR : read failed on socket for slave." <<endl;
    	
    	return -1;
    }

    return 1;
}

void requestSlave(string address, char command[], char response[]) 
{
    int socket_fd;
    int connection_status;

    //strcpy(message, "REQ\n1\n1\ngetstaticparams\n\n");

    socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (socket_fd < 0) {
        cout <<"Error creating socket" <<endl;
        exit(1);
    }

    connection_status = createConnectionWithSlave(socket_fd, address);

    if (connection_status < 0) {
        cout <<"ERROR: Connection failed with slave : " <<address <<endl;
        cout <<"moving on..." <<endl <<endl;
    } else {
        if (writeToSlave(socket_fd, command)) {
            if (readFromSlave(socket_fd, response)) {
                ;
            }
        }
    }
}

int createConnectionWithSlave(int socket_fd, string address_non_compatible)
{
    struct sockaddr_in slave_address;

    struct hostent *server;

    int port_number;
    int socket_return_value;

    char *address;

    cout <<"Enter the port number" <<endl;
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