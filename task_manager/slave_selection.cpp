#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <fstream>
#include <boost/tokenizer.hpp>
#include <sqlite3.h>
/* add other necessary headers */

using namespace std;

extern vector<vector<node>> task_vector;	// task vector from node.cpp
extern sqlite3* slave_db;	// slave db from databaseCRUD.cpp
extern void updateTable(string, double, double); 	// from databaseCRUD.cpp; send IP, LM, LP
extern string selectQuery();	// returns IP with minimum LM + LP 
extern int createConnectionWithSlave(int, string);	// from socket_connection.cpp
extern void requestSlave(string, char[], char[]);	// from socket_connection.cpp
extern int createResultSocketConnection(int&, int); // from socket_connection.cpp 

//Hack
extern map<string, int> moduleid_to_modulename_map;

void dfDistribute();	// entry point for this file
string slaveSelect();	// helper function.

int parseResponse(char*, double&, double&);

string getJobString(int module_id)
{
	string file_name;
	string line;
	string job_string;

	ifstream input;

	file_name = moduleid_to_modulename_map[module_id];

	input.open(file_name, ifstream::in);

	if (input.is_open()) {
		while(getline(input, line)) {
			job_string.append(line);
		}
	}

	input.close();

	return job_string;
}

void parseResultFromResponse(stirng result_string, vector<string>& result)
{
	typedef boost::tokenizer< boost::char_separator<char> > tokenizer;
	boost::char_separator<char> sep{"~"};

	tokenizer tok{response, sep};

	for (const auto &t : tok) {
    	result.push_back(t);
    }
}

void storeResult(char *response)
{
	string response_string(response);
	string file_name;

	vector<string> result_vector;

	fstream output;

	//tokeniZe the string to get the module_id and the result.
	parseResultResponse(response_string, result_vector);

	file_name.append(to_string(result_vector.at(0))).append("_result.scah");

	output.open(file_name, ios::out);

	if (output.is_open()) {
		output <<result_vector.at(1);
	} else {
		cout <<"output file failed to open" <<endl;
	}

}
void dfDistribute()
{
	string IP_of_bpsm;	// ip address of best possible slave machine
	string currentIP;	// for result polling

	char command[128];
	char response[128];

	string interm_command = "JOB\n2\n2\n";

	int counter;
	int result_socket;
	int result_port;

	cout <<"Enter the port number for the results" <<endl;
	cin >>result_port;

	// level-wise traversal
	for(int i = 0; i < task_vector.size(); i++) {	
		for(int j = 0; j < task_vector[i].size(); j++) {
			IP_of_bpsm = slaveSelect();

			bzero(command, 128);
			bzero(response, 128);
			interm_command.append(to_string(task_vector[i][j].module_id)).append(" ");
			interm_command.append(getJobString(task_vector[i][j].module_id)).append("\n\n");

			strcpy(command, interm_command.c_str());

			requestSlave(IP_of_bpsm, command, response);	// send job here itself, modify command
			// now ignore response! no need for any parsing
		}

		counter = task_vector[i].size();

		while (counter != 0) {
			// Create a socket connection for the results and wait for the slave to respond..
			if ((createResultSocketConnection(result_socket, result_port)) > -1) {
				// TEST CODE
				cout <<"The slave has contacted the master" <<endl;

				bzero(response, 128);
				// Fetch the response from the slave.
				if ((readFromSlave(result_socket, response)) == -1) {
					cout <<"Error reading result response from the slave" <<endl
				} else {
					// Parse the result string.
					// Store the result string in a file module_id_result.scah
					storeResult(response); 
					
				}
			} else {
				cout <<"Creation of socket failed while listening to the response" <<endl;
			}
			// Decrement the counter as we recieve a response.
			counter--; 
		}
	}
	cout << "Application run complete. Check scah.output to check output.\n"
}

int parseResponse(char *response)
{
	string response_from_execution;
	string number_of_results;
	vector<string> cmd;
	vector<string> results;
	string response(response_command);

	typedef boost::tokenizer< boost::char_separator<char> > tokenizer;
	boost::char_separator<char> sep{"\n"};

	tokenizer tok{response, sep};

	for (const auto &t : tok) {
    	cmd.push_back(t);
    }

    number_of_results = cmd.at(2);

    response_from_execution = cmd.at(3);

    if (response_from_execution == "SUCCESS") {
    	return 1;
    } else {
    	return 0;
    }
}

int parseResponse(char *response_command, double &LM, double &LP)
{
	string response_from_execution;
	string number_of_results;
	vector<string> cmd;
	vector<string> results;
	string response(response_command);

	typedef boost::tokenizer< boost::char_separator<char> > tokenizer;
	boost::char_separator<char> sep{"\n"};

	tokenizer tok{response, sep};

	for (const auto &t : tok) {
    	cmd.push_back(t);
    }

    number_of_results = cmd.at(2);

    response_from_execution = cmd.at(3);

    cout <<response_from_execution <<endl;

    typedef boost::tokenizer< boost::char_separator<char> > tokenizer1;
	boost::char_separator<char> sep1{"\t"};

	tokenizer1 tok1{response_from_execution, sep1};

	for (const auto &t : tok1) {
    	results.push_back(t);
    }

    LM = (double)atoi(results.at(0).c_str());
    LP = atof(results.at(1).c_str());

    return 1;
}

string slaveSelect()
{
	double LM;	// memory util
	double LP;	// CPU util

	string address;
	ifstream input;

	char command[128];
	char response[128];

	bzero(command, 128);
	bzero(response, 128);

	strcpy(command, "REQ\n1\n\2\ngetutilizations\n\n");

	input.open("discovered_hosts", ifstream::in);

	if (input.is_open()) {
		while(getline(input, address)) {
			requestSlave(address, command, response);
			parseResponse(response, LM, LP);

			updateTable(address, LM, LP);
		}
	}

	return selectQuery();
}