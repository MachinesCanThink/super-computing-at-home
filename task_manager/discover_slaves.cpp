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

#define PORT 44124
#define FALLBACK_PORT 44223

void addSlaveNodeToFile(string);
void discoverSlaveNodes();
int createConnectionWithSlave(int, string);
int writeToSlave(int, char*);
int readFromSlave(int, char*);

int main(int argc, char* argv[]) {
	discoverSlaveNodes();

	return 0;
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

void addSlaveNodeToFile(string address)
{
	ofstream output;

	output.open("discovered_slaves", ifstream::app);

	if (output.is_open()) {
		output <<address <<endl;
	}
}

int createConnectionWithSlave(int socket_fd, string address_non_compatible)
{
	struct sockaddr_in slave_address;

    struct hostent *server;

    int port_number;
	int socket_return_value;

	char *address;
	char response_message[64];

	port_number = PORT;

	bzero(response_message, 64);

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

void discoverSlaveNodes() {
	string address;
	char message[64];
	char response_message[64];

	int socket_fd;
	int connection_status;

    ifstream input;

    bzero(message, 64);
    bzero(response_message, 64);

   	strcpy(message, "CHECK");

    socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (socket_fd < 0) {
        cout <<"Error creating socket" <<endl;
    	exit(1);
    }

    input.open("discovered_hosts", ifstream::in);

    // Read the contents of the file and obtain the IP address's to connect to. 
    if (input.is_open()) {
    	while(getline(input, address)) {
    		cout <<address <<endl;
    		cout <<message <<endl;

    		connection_status = createConnectionWithSlave(socket_fd, address);

    		if (connection_status < 0) {
    			cout <<"ERROR: Connection failed with slave : " <<address <<endl;
    			cout <<"moving on..." <<endl <<endl;
    		} else {
    			if (writeToSlave(socket_fd, message)) {
    				if (readFromSlave(socket_fd, response_message)) {
    					cout <<response_message <<endl;

    					addSlaveNodeToFile(address);
    				}
    			}
    		}
    	}
    }
}   