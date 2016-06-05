#include <iostream>
#include <cstdlib>
#include <string>
#include <cstring>
#include <fstream>

using namespace std;

int discovered_hosts(void);
int discover(string);

int discover_hosts(void) 
{
	string address;

	cout <<"Enter the IP address of the router" <<endl;
	cin >>address;

	discover(address);

	return 0;
}

int discover(string ip_address)
{
	string command_interm;
	int status;
	char *command;
	int count  = 0;

	int i, j;

	string line;
	ifstream input;
	ofstream output;

	command_interm = "sudo nmap -sP --send-ip -oG discovered_hosts.tmp ";

	command_interm.append(ip_address);
	cout <<"command: " <<command_interm <<endl;

	command =new char[command_interm.length() + 1];
	strcpy(command, command_interm.c_str());

	system(command);

	input.open("discovered_hosts.tmp", ifstream::in);
	output.open("discovered_hosts", ifstream::out);

	if (input.is_open()) {
        while (getline(input, line)) {
           	if (line[0] == '#') {
 				;
           	} else {
           		string sub_text;
           		for (i = 6, j = 0; i < line.size(); i++, j++) {
           			if (line[i+1] != '(') 
           				sub_text.push_back(line[i]);
           			else
           				break; 	
           		}

           		if (output.is_open())
           			output <<sub_text <<endl;
           	}
        }
    }

    input.close();
    output.close();

	return 1;
}
