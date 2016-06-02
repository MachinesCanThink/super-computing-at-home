#include <iostream>
#include <vector>
#include <cstring>
#include <string>
#include <cstdlib>
#include <syslog.h>

#include "system_spec.h"

string getUtilizations(void);
int getStaticParams(void);

extern system_spec specs;

string getStaticParams(void)
{
	string hyperthreading_info;
	string num_cores;
	string total_mem;

	string result;

	// The system_spec.cpp program already calcuates this for us and this we need not worry about the helper methods. 
	// Just filling in the vector here by using struct system_specs defined in system_spec.h file.
	syslog(LOG_NOTICE, "HTT: %d\n", specs.is_hyperthreaded);
	syslog(LOG_NOTICE, "number of cores: %d\n", specs.number_of_cores);
	syslog(LOG_NOTICE, "total memory: %d\n", specs.total_memory);

	if (specs.is_hyperthreaded == 1) {
		hyperthreading_info = "true\n";
	} else {
		hyperthreading_info = "false\n";
	}

	num_cores = to_string(specs.number_of_cores).append("\n");

	total_mem = to_string(specs.total_memory).append("\n");

	// Now we concatinate the strings so that it can be sent via a TCP connection.
	result.append(hyperthreading_info).append(num_cores).append(total_mem);

	return result;
}

int getUtilizations(void)
{

}
