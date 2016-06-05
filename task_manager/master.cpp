#include <iostream>
#include <cstdlib>
#include <string>

extern int parser(int, char*[]);
extern int discover_hosts(void);
extern int discover_slaves(void);
extern sqlite3* slave_db;

int main(int argc, char *argv[])
{
	parser(argc, argv);

	discover_hosts();

	// create a database
	if (sqlite3_open(NULL, slave_db))
		cout << "error: sqlite: unable to create db\n";
	
	discover_slaves();



	return 0;
}