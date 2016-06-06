#include <iostream>
#include <vector>
#include <cstring>
#include <string>
#include <cstdlib>
#include <syslog.h>

#include "system_spec.h"

using namespace std;

extern double getMemoryUtilization(void);
extern double getCpuUtilization(void);
extern void prepareSystemSpecDS(void);

string getUtilizations(void);
string getStaticParams(void);
string isExecutionComplete(void);

extern system_spec specs;

string getStaticParams(void)
{
	string hyperthreading_info;
	string num_cores;
	string total_mem;

	string result;

	prepareSystemSpecDS();

	// The system_spec.cpp program already calcuates this for us and this we need not worry about the helper methods. 
	// Just filling in the vector here by using struct system_specs defined in system_spec.h file.
	syslog(LOG_NOTICE, "HTT: %d\n", specs.is_hyperthreaded);
	syslog(LOG_NOTICE, "number of cores: %d\n", specs.number_of_cores);
	syslog(LOG_NOTICE, "total memory: %d\n", specs.total_memory);

	if (specs.is_hyperthreaded == 1) {
		hyperthreading_info = "true ";
	} else {
		hyperthreading_info = "false ";
	}

	num_cores = to_string(specs.number_of_cores).append(" ");

	total_mem = to_string(specs.total_memory).append("\n");

	// Now we concatinate the strings so that it can be sent via a TCP connection.
	result.append(hyperthreading_info).append(num_cores).append(total_mem);

	return result;
}

string getUtilizations(void)
{
	double memory_utilization;
	double cpu_utilization;

	string utilizations;

	memory_utilization = getMemoryUtilization();
	cpu_utilization = getCpuUtilization();

	utilizations = to_string(memory_utilization).append(" ").append(to_string(cpu_utilization)).append("\n");

	return utilizations;
}

