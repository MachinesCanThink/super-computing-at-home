#include <boost/tokenizer.hpp>
#include <boost/foreach.hpp>
#include <boost/regex.hpp>
#include <iostream>
#include <cstdlib>
#include <string>
#include <cstring>
#include <vector>
#include <map>
#include <syslog.h>

using namespace std;

int parseCommandFromMaster(char*);
int getFunctionIDFromName(string);
int getIdOfTheIncomingModule();
string getFunctionToCall(string);
string convertResultToString(int);
string convertResultToString(double);
string callFunctionByID(int);

string getStaticParams();
string getUtilizations();
string prepareJobFile(int);

void initFunctionNamesMap(void);
void getFunctionNamesFromCmd(vector<string>);

vector<string> function_names_from_string;
map<string, int> function_names;

int module_id_latest;


void initFunctionNamesMap(void)
{
	function_names["getUtilizations"] = 1;
	function_names["getStaticParams"] = 2;
	function_names["prepareJobFile"] = 3;
}

int getIdOfTheIncomingModule(void)
{
	return 1;
}

int getFunctionIDFromName(string function_name)
{
	int id;

	id = function_names[function_name];

	return id;
}

string prepareJobFile(int module_id)
{
	return "Null";
}

string getUtilizations()
{
	return "Null";
}

string getStaticParams()
{
	return "null";
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
		case 3: {
			string result;

			int module_id;

			module_id = getIdOfTheIncomingModule();
			result = prepareJobFile(module_id);

			return result;
		}
		default : {
			return "Null";
		}
	}
}

/*
bool isNumber(string number)
{
	boost::regex expression{"^-?\\d+"};
	//boost::match_results<std::string::const_iterator> what;

	if (regex_match{number, expression}) {
		return true;
	} else {
		return false;
	}
}
*/

void getFunctionNamesFromCmd(vector<string> commands)
{
	int iterator;
	int iterator2;

	for (iterator = 3, iterator2 = 0; iterator < commands.size(); iterator++, iterator2++) {
		function_names_from_string.push_back(commands.at(iterator));
	}

	cout <<"Function names:" <<endl;

	for (iterator = 0; iterator < function_names_from_string.size(); iterator++) {
		cout <<function_names_from_string.at(iterator) <<endl;
	}
}

int parseCommandFromMaster(char *message)
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

	int num_functions;

	// (REQ | RES) ? (0 | 1)
	int command_type;

	// Obtained from the request string. Used as an identifier.
	int unique_number;

	status = true;

	tokenizer tok{request, sep};

	for (const auto &t : tok) {
    	cmd.push_back(t);
    }

    // parse the vector of commands.
    // set the message type.
    if (cmd.at(0) == "RES") {
    	command_type = 1;
    } else if (cmd.at(0) == "REQ") {
    	command_type = 0;
    } else if (cmd.at(0) == "JOB") {
    	command_type = 2;
    } else if (cmd.at(0) == "FILE") {
    	command_type = 3;
    } else {
    	cout <<"blah1" <<endl;
    	status = false;
    }

    /*
    if (isNumber(cmd.at(1))) {
    	// store the unique number and verify if it's a number or not.
    	unique_number = atoi(cmd.at(1).c_str());
	} else {
		status = false;
	}

	if (isNumber(cmd.at(2))) {
   		// Get the number of funtions to be run and verify if it's a number or not.
    	num_functions = atoi(cmd.at(2).c_str());
	} else {
		status = false;
	}
	*/

	// TEST CODE
	unique_number = atoi(cmd.at(1).c_str());
	num_functions = atoi(cmd.at(2).c_str());

    if (status) {
    	cout <<"command type: " <<command_type <<endl;
    	cout <<"Unique number: " <<unique_number <<endl;
    	cout <<"number of functions: " <<num_functions <<endl;	
    }

    getFunctionNamesFromCmd(cmd);

    

	return (int)status;
}

int main()
{
	char message[] = "REQ\n123\n2\nfunc1\nfun2\n\n";

	//initMessageTypeMap();
	parseCommandFromMaster(message);

	return 0;
}