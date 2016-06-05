#include <iostream>
#include <stdlib.h>
#include <sqlite3.h>
#include <sstream>

using namespace std;

sqlite3* slave_db;

void createTable()
{
	char* createCommand = "CREATE TABLE SLAVES("  \
         				  "IP CHAR[15] PRIMARY KEY NOT NULL," \
         				  "RAM REAL NOT NULL," \
         				  "HTT INT, " \
         				  "NC INT, " \
         				  "LM REAL," \
         				  "LP REAL);";

    if (sqlite3_exec(slave_db, createCommand, NULL, 0, NULL)){
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
//}

void insertIntoTable(string IP, double RAM, int HTT, int NC, double LM = 0.0, double LP = 0.0)
{
	const char* insertCommand = "INSERT INTO SLAVES VALUES ('" + IP + "', '" + to_string(RAM) + "', '" + to_string(HTT) + "', '" + to_string(NC) + "', '"\
								+ to_string(LM) + "', '" + to_string(LP) + "');";

	if (sqlite3_exec(slave_db, insertCommand, NULL, 0, NULL)){
    	cout << "error: sqlite: cannot execute insert command\n";
    	exit(0);
    }  
}

