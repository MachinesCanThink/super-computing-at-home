#include <boost/tokenizer.hpp>
#include <boost/foreach.hpp>
#include <boost/regex.hpp>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <cstring>
#include <vector>
#include <map>
#include <syslog.h>
#include <pthread.h>

using namespace std;

typedef struct command {
	int command_type;
	int unique_number;
	int number_of_functions;

	vector<string> function_names_from_string;
	vector<string> result;
} command_struct;

typedef struct job {
	string file_name;
	int module_id;

	string result;
} job;

// TEST CODE
string getStaticParams();
string getUtilizations();
string prepareJobFile(int);

int parseCommandFromMaster(char*);
int getFunctionIDFromName(string);
int getModuleIDFromCommand(command_struct);
int prepareResponsePacket(command_struct&, string&);
int executeCommandFunctions(command_struct&);
int copyJobToFile(string);
int createFileForIncomingJob(string);
string getFileNameFromCommand(command_struct);
string getJobStringFromCommand(command_struct);
string getFunctionToCall(string);
string callFunctionByID(int);
string prepareResponsePacket(vector<string>, vector<string>);
void initFunctionNamesMap(void);
void getFunctionNamesFromCmd(vector<string>);

//Function pointers
void *executeJob(void *);

extern bool is_job;

map<string, int> function_names;

job new_job;

void initFunctionNamesMap(void)
{
	function_names["getutilizations"] = 1;
	function_names["getstaticparams"] = 2;
	function_names["preparejobfile"] = 3;
}

int getFunctionIDFromName(string function_name)
{
	int id;

	id = function_names[function_name];

	return id;
}

string callFunctionByID(int id)
{
	switch (id) {
		case 1: {
			string result;

			result = getUtilizations();

			return result;
		}
		case 2: {
			string result;

			result = getStaticParams();

			return result;
		}
		default : {
			return "Null";
		}
	}
}

void getFunctionNamesFromCmd(vector<string> commands, command_struct &command)
{
	int iterator;
	int iterator2;

	for (iterator = 3, iterator2 = 0; iterator < commands.size(); iterator++, iterator2++) {
		command.function_names_from_string.push_back(commands.at(iterator));
	}
}

int prepareResponsePacket(command_struct &command, string &response_string)
{
	string response_type;
	string number_of_results;
	string unique_number;

	if (command.command_type == 0) {
		// command is request. We should respond.
		response_type = "RES";
	} else if (command.command_type == 1) {
		//command type is Response. generally we ignore and simply use the result. likely to not happen.
		;
	} else if (command.command_type == 2) {
		// command type is FILE. so we respond with a success or failure message.
		// response type will simply be a RES.
		response_type = "RES";
	} else if (command.command_type == 3) {
		//command type, JOB. response type will be RESULT.
		response_type = "RESULT";
	} else {
		// I dont know what this is, lol.
		response_type = "Null";
	}

	unique_number = to_string(command.unique_number);

	number_of_results = to_string(command.number_of_functions);

	response_string = response_string.append(response_type).append("\n").append(unique_number).append("\n");

	response_string.append(number_of_results).append("\n");

	for (int i = 0; i < command.result.size(); i++) {
		response_string.append(command.result.at(i));
	}

	response_string.append("\n");

	return 1;
}

string getFileNameFromCommand(command_struct command)
{
	return command.function_names_from_string.at(1);
}

string getJobStringFromCommand(command_struct command)
{
	return command.function_names_from_string.at(0);
}

int getModuleIDFromCommand(command_struct command)
{
	return atoi(command.function_names_from_string.at(0).c_str());
}

int createFileForIncomingJob(string file_name)
{
	bool status;

	fstream job_file;

	job_file.open(file_name, ios::out);

	job_file <<flush;

	if (job_file.is_open()) {
		status = true;
	} else {
		status = false;
	}

	job_file.close();

	return (int)status;
}

int copyJobToFile(string job_string)
{
	string file_name;
	fstream job_file;

	file_name = new_job.file_name;

	job_file.open(file_name, ios::out);

	if (job_file.is_open()) {
		job_file <<job_string;

		return true;
	} else {
		return false;
	}

	job_file.close();
}

int executeJob(void)
{
	string command;
	string file_name;
	string result_file;

	command = "python ";

	file_name = new_job.file_name;
	result_file = "result_";

	result_file.append(file_name).append(".scah");

	command.append(file_name).append(" > ").append(result_file);

	system(command.c_str());

	return 1;

}

string getResultFromOutputFile(void) 
{
	string result;
	string input_file;
	string line;

	ifstream input;

	input_file = "result_";
	input_file.append(new_job.file_name).append(".scah");

	input.open(input_file, ifstream::in);

	if (input.is_open()) {
		while(getline(input, line)) {
			result.append(line).append(" ");
		}
	}

	return result;
}

int executeCommandFunctions(command_struct &command) 
{
	bool status;

    switch (command.command_type) {
    	case 0: {
    		initFunctionNamesMap();

    		for (int i = 0; i < command.function_names_from_string.size(); i++) {
    			int id = getFunctionIDFromName(command.function_names_from_string.at(i));

    			command.result.push_back(callFunctionByID(id));
    		}

    		if (command.result.size() > 0) {
    			status = true;
    		} else {
    			status = false;
    		}

    		break;
    	}
    	case 1: {

    		break;
    	}
    	case 2: {
    		string file_name;
    		int module_id;

    		// Incoming file. 
    		// Prepare the file by getting the file name from then command.function_names_from_string,at(0).

    		file_name = getFileNameFromCommand(command);

    		module_id = getModuleIDFromCommand(command);

    		new_job.module_id = module_id;
    		new_job.file_name = file_name;

    		if (createFileForIncomingJob(file_name)) {
    			syslog(LOG_NOTICE, "File created. Response packet will be created.");
    			cout <<"File created" <<endl;
    			status = true;

    			new_job.result = "Null";
    			command.result.push_back("success");
    		} else {
    			status = false;
    			syslog(LOG_NOTICE, "File creation failed");
    		}

    		break;
    	}
    	case 3: {
    		// Have recieved the job now and now we execute the job here by placing the job string in the file.
    		string job_string;
    		string result_string;

    		job_string = getJobStringFromCommand(command);

    		if (copyJobToFile(job_string)) {
    			status = true;
    			syslog(LOG_NOTICE, "Job string copied on to the file");
    		} else {
    			status = false;
    			syslog(LOG_NOTICE, "job string not copied");
    		}

    		if (executeJob()) {
    			syslog(LOG_NOTICE, "Job finished execution");

    			result_string = getResultFromOutputFile();

    			command.result.push_back(result_string);
    		} else {
    			syslog(LOG_NOTICE, "Job did not run");
    		}

    		break;
    	}
    }
    return (int)status;
}

int parseCommandFromMaster(char *message, command_struct &command)
{
	bool status;

	vector<string> cmd; 

	string request(message);

	typedef boost::tokenizer< boost::char_separator<char> > tokenizer;
	boost::char_separator<char> sep{"\n"};

	// The char* has been converted to std::string. 
	// Makes my life easier now.
	/*
	 * General Format:
	 * Request: 
	 * REQ\nUNIQUE_NUM\nNUM_COMMAND\nCOMMAND1\nCOMMAND2\nCOMMAND3\n\n
	 * \n is used as a delimiter and \n\n indicates the end of the req string.
	 * Response: 
	 * RES\nUNIQUE_NUM\nNUM_COMMAND\nRes1\nRes2\nRes3\n\n
	 */
	status = true;

	tokenizer tok{request, sep};

	for (const auto &t : tok) {
    	cmd.push_back(t);
    }

    // parse the vector of commands.
    // set the message type.
    if (cmd.at(0) == "RES") {
    	command.command_type = 1;
    } else if (cmd.at(0) == "REQ") {
    	command.command_type = 0;
    } else if (cmd.at(0) == "JOB") {
    	command.command_type = 3;
    } else if (cmd.at(0) == "FILE") {
    	command.command_type = 2;
    } else {
    	status = false;
    }

	// TEST CODE
	command.unique_number = atoi(cmd.at(1).c_str());
	command.number_of_functions = atoi(cmd.at(2).c_str());
	
	// Store the names of the functions in a vector. 
    getFunctionNamesFromCmd(cmd, command);

	return (int)status;
}

int processRequestFromMaster(char *command_string, char *response_string)
{
	bool status;

	string response_packet;

	command_struct command;

	//initMessageTypeMap();
	parseCommandFromMaster(command_string, command);

	//Run the functons now.
    if ((executeCommandFunctions(command))) {
    	syslog(LOG_NOTICE, "Function execution successful");
    } else {
    	syslog(LOG_NOTICE, "Function execution fails");
    }

    // We now prepare the response packet that is sent back to the master.
    if ((prepareResponsePacket(command, response_packet))) {

    	strcpy(response_string, response_packet.c_str());

    	syslog(LOG_NOTICE, "packet prepared");
    } else {
    	syslog(LOG_NOTICE, "error in preparing packet");

    	response_packet = "Null";

    	status = false;
    }

	return (int) status;
}

/*
int main(void)
{
	char message[128]; 
	char response_string[128];

	bzero(message, 128);
	bzero(response_string, 128);

	strcpy(message, "REQ\n1\n2\ngetstaticparams\ngetutilizations\n");

	processRequestFromMaster(message, response_string);

	cout <<response_string;

	bzero(message, 128);
	bzero(response_string, 128);

	strcpy(message, "FILE\n2\n2\n101\ntest_module.py\n\n");

	processRequestFromMaster(message, response_string);

	cout <<response_string;

	bzero(message, 128);
	bzero(response_string, 128);

	strcpy(message, "JOB\n3\n1\nprint 3-3+2\n\n");

	processRequestFromMaster(message, response_string);

	cout <<response_string;

	return 0;
}
*/