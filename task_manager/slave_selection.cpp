#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <fstream>
#include <boost/tokenizer.hpp>
#include <sqlite3.h>
/* add other necessary headers */

using namespace std;

//extern vector<vector<node>> task_vector;	// task vector from node.cpp
extern sqlite3* slave_db;	// slave db from databaseCRUD.cpp
//extern void updateTable(string, double, double); 	// from databaseCRUD.cpp; send IP, LM, LP
extern string selectQuery();	// returns IP with minimum LM + LP 
extern int createConnectionWithSlave(int, string);	// from socket_connection.cpp
extern void requestSlave(string, char[], char[]);	// from socket_connection.cpp

//Hack
extern map<string, int> moduleid_to_modulename_map;

void dfDistribute();	// entry point for this file
string slaveSelect();	// helper function.

int parseResponse(char*, double&, double&);

void dfDistribute()
{
	string IP_of_bpsm;	// ip address of best possible slave machine
	string currentIP;	// for result polling
	char command[128];
	char response[128];
	string interm_command = "FILE\n2\n2\n";

	// level-wise traversal
	for(int i = 0; i < task_vector.size(); i++) {	
		for(int j = 0; j < task_vector[i].size(); j++) {
			if(task_vector[i][j].migratable) {	/* add bool migratable in node struct */
				IP_of_bpsm = slaveSelect();
				interm_command.append(moduleid_to_modulename_map[task_vector[i][j].module_id]).append("\n\n");
				bzero(command, 128);
				strcpy(command, interm_command.c_str());

				bzero(response, 128);

				requestSlave(IP_of_bpsm, command, response);

				if (parseResponse(response)) {
					// Create thread for sending job and then move on. 
				} else {
					cout <<"Slave failed at creating the file." <<endl;
				}
			} else {
				// execute the task_vector[i][j] 
				// write result + module ID to scah.output
				// case shall not arise for simulation
				// ignore
			}
		}
		// result polling for level-i
		/*
		for(/*loop through discovered_slaves file*///) {
			//currentIP = /*line of discovered_slaves file*/
			/*bzero(command, 128);
			strcpy(command, "REQ\n1\n1\n")
			requestSlave(currentIP, command, response);
			
			/* parse response */
			/*if(/*resultStatus is COMPLETE*///) {
				//command = /*REQ for result*/
				//requestSlave(currentIP, command, response);
				/* parse response to get result and module id
				 * write result + module ID to scah.output */
			/*} else
				continue;
		}*/
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

			//updateTable(address, LM, LP);
		}
	}

	//return selectQuery();	// SELECT QUERY DOES NOT WORK. I still need to figure the select query, wait until i do.
	return "nlah";
}