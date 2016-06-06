#include <iostream>
#include <cstdlib>
#include <string>
/* add other necessary headers*/

extern int parser(int, char*[]);
extern int discover_hosts(void);
extern int discover_slaves(void);
extern sqlite3* slave_db;
extern void dfDistribute();	// see slaveSelection.cpp 

int main(int argc, char *argv[])
{
	parser(argc, argv);

	discover_hosts();

	// create a database
	if (sqlite3_open(NULL, slave_db))
		cout << "error: sqlite: unable to create db\n";
	
	discover_slaves();
	dfDistribute();

	return 0;
}