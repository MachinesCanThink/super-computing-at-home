#include <iostream>
#include <stdlib.h>
#include <sqlite3.h>
#include <sstream>

using namespace std;

sqlite3* slave_db;

void createTable();
void insertIntoTable(string, double, int, int, double, double);
void updateTable(string, double, double);
string selectQuery();

void createTable()
{
	char* create_command = "CREATE TABLE SLAVES("  \
         				  "IP CHAR[15] PRIMARY KEY NOT NULL," \
         				  "RAM REAL NOT NULL," \
         				  "HTT INT, " \
         				  "NC INT, " \
         				  "LM REAL," \
         				  "LP REAL);";

    if (sqlite3_exec(slave_db, create_command, NULL, 0, NULL)){
    	cout << "error: sqlite: cannot execute create command\n";
    	exit(0);
    }
}


// This function converts a double to string and returns the string
// string toString(double db)
// {
// 	ostringstream strs;
//     strs << db;
// 	return strs.str();
// }

void insertIntoTable(string IP, double RAM, int HTT, int NC, double LM = 0.0, double LP = 0.0)
{
	const char* insert_command = "INSERT INTO SLAVES VALUES ('" + IP + "', '" + to_string(RAM) + "', '" + to_string(HTT) + "', '" + to_string(NC) + "', '"\
								+ to_string(LM) + "', '" + to_string(LP) + "');";

	if (sqlite3_exec(slave_db, insert_command, NULL, 0, NULL)){
    	cout << "error: sqlite: cannot execute insert command\n";
    	exit(0);
    }  
}

void updateTable(string IP, double LM, double LP) 
{
	const char* update_command = "UPDATE SLAVES set LM = '" + to_string(LM) + "' where IP = '" + IP + "'; " \
							     "SELECT * from COMPANY";

    if (sqlite3_exec(slave_db, update_command, NULL, 0, NULL)) {
    	cout << "error: sqlite: cannot execute update command\n";
    	exit(0);
    }					
}

string selectQuery()	// modify
{
	const char* select_command = "SELECT IP from SLAVES WHERE LM + LP is minimum";	// modify
	
	if (sqlite3_exec(slave_db, update_command, NULL, 0, NULL)) {
    	cout << "error: sqlite: cannot execute update command\n";
    	exit(0);
    }	

    return IP;
}
