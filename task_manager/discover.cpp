#include <iostream>
#include <cstdlib>
#include <string>
#include <cstring>
#include <fstream>
#include <regex>

using namespace std;

int discover(string);

int main(int argc, char *argv[]) 
{
	discover("192.168.0.1/24");

	return 0;
}

int discover(string ip_address)
{
	string command_interm;
	int status;
	char *command;
	//regex regex_exp;
	smatch match;
	string line;
	ifstream input;
	ofstream output;

	//regex regex_exp("\b\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3}\b");

	command_interm = "sudo nmap -sP --send-ip -oG discovered_hosts.tmp ";

	command_interm.append(ip_address);
	cout <<"command: " <<command_interm <<endl;

	command =new char[command_interm.length() + 1];
	strcpy(command, command_interm.c_str());

	system(command);

	//system("grep -E -o '(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)' discovered_hosts.tmp > discovered_hosts");

	/*
	ifstream input_file("discovered_hosts.tmp");
	ostream output_file("discovered_hosts");

	while (getline(input_file, s)) {
		if (regex_search(s.begin(), s.end(), match, regex_exp)) {
			output_file <<match[1] <<endl;
		}
	}
	*/

	input.open("discovered_hosts.tmp");
	output.open("discovered_hosts");

	if (input.is_open()) {
        while (getline(input, line)) {
           	if (line[0] == '#') {
           		;
           	} else {
           		string sub_text;
           		for (int i = 6; i < line.size(); i++) {
           			if (line[i+1] != '(')
           				sub_text[i] = line[i];
           			else
           				break; 	
           		}

           		cout <<sub_text <<endl;
           		output <<sub_text <<endl;
           	}
        }
    }

    input.close();
    output.close();

    //system("grep -E -o '(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)' discovered_hosts.tmp1 > discovered_hosts");

	return 1;
}
