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

void addSlaveNodeToFile(string);
extern void requestSlave(string, char[], char[]);
extern int writeToSlave(int, char*);
extern int readFromSlave(int, char*);

/* createTable and insertIntoTable declared from databaseCRUD.cpp */
extern void createTable(void);
extern void insertIntoTable(string, double, int, int, double, double);

int discover_slaves(void);

int discover_slaves(void) 
{
    // create slave table in the database
    createTable();

    string address;
    ifstream input;

    char command[128];
    char response[128];

    bzero(command, 128);
    bzero(response, 128);

    input.open("discovered_hosts", ifstream::in);

    strcpy(command, "REQ\n1\n1\ngetstaticparams\n\n");

    // Read the contents of the file and obtain the IP address's to connect to. 
    if (input.is_open()) {
        while(getline(input, address)) {
            //cout <<address <<endl;

            requestSlave(address, command, response);

            string IP   = address;
            double RAM = // parse response for RAM
            int HTT    = // parse response for HTT (bool isnt available in sqlite)
            int NC     = // parse response for NC 
            
            insertIntoTable(IP, RAM, HTT, NC);
        }
    }

	return 0;
}

void addSlaveNodeToFile(string address)
{
	ofstream output;

	output.open("discovered_slaves", ifstream::app);

	if (output.is_open()) {
		output <<address <<endl;
	}
}





     