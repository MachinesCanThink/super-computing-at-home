#include <iostream>
#include <string>
#include <cstdlib>
#include <fstream>
#include <ctype.h>
#include <syslog.h>
#include <thread>

#include "system_spec.h"

using namespace std;

system_spec specs;

system_spec getSystemSpecs();
int getNumberOfCores(system_spec&);
int getHyperthreadingInfo(system_spec&);
int getTotalMemory(void);
int getNumberOfPhysicalCores(void);
int getInstructionsPerSeconds(void);
double getCpuUtilization(void);
double getMemoryUtilization(void);
void initializeSpecStruct(system_spec&);
void prepareSystemSpecDS(void);

void initializeSpecStruct(system_spec &spec)
{
	spec.is_hyperthreaded = 0;
	spec.number_of_cores = -1;
	spec.total_memory = -1;
}

int getInstructionsPerSeconds(void)
{
	int result;

	result = 0;
	
	return result;
}

int getNumberOfCores(system_spec &spec)
{
	int num_CPU;

	num_CPU = thread::hardware_concurrency();

	spec.number_of_cores = num_CPU;

	return 0;
}

int getTotalMemory(system_spec &spec)
{
	int total_memory;
	int counter;
	int status;

	int i, j;
	ifstream input;

	string line;
	string numbers;

	counter = 0;

	input.open("/proc/meminfo", ifstream::in);

	if (input.is_open()) {
		while(getline(input, line)) {
			if (counter == 1) {
				break;
			}

			string line1 = line;

			for (i = 0, j = 0; i < line1.size(); i++) {
				if (isdigit(line1[i])) {
					numbers[j++] = line1[i];
				}
			}

			counter++;
		}
	}

	total_memory = atoi(numbers.c_str());

	spec.total_memory = total_memory / 1024;

	input.close();

	return 1;
}

// Overrided method to call when calculating the memory utilization.
int getTotalMemory(void)
{
	int total_memory;
	int counter;
	int status;

	int i, j;
	ifstream input;

	string line;
	string numbers;

	counter = 0;

	input.open("/proc/meminfo", ifstream::in);

	if (input.is_open()) {
		while(getline(input, line)) {
			if (counter == 1) {
				break;
			}

			string line1 = line;

			for (i = 0, j = 0; i < line1.size(); i++) {
				if (isdigit(line1[i])) {
					numbers[j++] = line1[i];
				}
			}

			counter++;
		}
	}

	total_memory = atoi(numbers.c_str()) / 1024;

	input.close();

	//spec.total_memory = total_memory / 1024;

	return total_memory;
}

int getNumberOfPhysicalCores(void)
{
	int cpu_count;
	int counter;

	int i, j;
	ifstream input;

	string number;

	input.open("/proc/cpuinfo", ifstream::in);
	counter = 0;
	cpu_count = 0;

	if(input.is_open()) {
		string line;
		string line1;
		while(getline(input, line)) {
			if (counter == 12) {
				line1 = line;
				number = line1[line1.size() - 1];
			}

			counter++;
		}
	}

	cpu_count = atoi(number.c_str());

	return cpu_count;
}

int getHyperthreadingInfo(system_spec &spec)
{
	int actual_number_of_cores;
	int number_of_cores;

	number_of_cores = thread::hardware_concurrency();

	actual_number_of_cores = getNumberOfPhysicalCores();

	if (number_of_cores == (2 * actual_number_of_cores)) {
		spec.is_hyperthreaded = 1;
	} else {
		spec.is_hyperthreaded = 0;
	}

	return 1;
}

system_spec getSystemSpecs(void)
{
	system_spec spec;

	initializeSpecStruct(spec);

	if ((getNumberOfCores(spec)) == -1) {
		syslog(LOG_NOTICE, "Error getting number of cores");

		return spec;
	}

	if ((getHyperthreadingInfo(spec)) == -1) {
		// Will never happen IMO.
		syslog(LOG_NOTICE, "Error getting hyper threading information");

		return spec;
	}

	if ((getTotalMemory(spec)) == -1) {
		syslog(LOG_NOTICE, "Error getting total available memory");

		return spec;
	}

	return spec;
}

// The dynamic parameters part. 
double getCpuUtilization(void)
{
	string command;
	string file_name;
	string line;

	ifstream input;

	// This is a hack. No time to get it the proper way. Lel
	command = "sh cpu_util_script.sh";

	file_name = "cpu_util";

	input.open(file_name, ifstream::in);

	// Execute the command and store the value in a file called "cpu_util".
	system(command.c_str());

	if (input.is_open()) {
		while(getline(input, line)) {

			break;
		}
	}

	double utilization = atoi(line.c_str());

	input.close();

	return utilization;
}

double getMemoryUtilization(void)
{
	ifstream input;

	int line_counter;
	int i;
	int j;
	int total_memory;
	int free_mem;

	double utilization;

	string free_memory;

	/* We open the /proc/meminfo file and obtain the total and free 
	 * free memory values. 
	 * utilization = 1 - (free memory / total memory)
	 */

	input.open("/proc/meminfo", ifstream::in);
	line_counter = 0;

	if (input.is_open()) {
		string line;
		while(getline(input, line)) {
			if (line_counter == 0) {
				;
			} else if (line_counter == 1) {
				string line1 = line;
				//cout <<endl <<line1 <<endl;

				for (i = 0, j = 0; i < line1.size(); i++) {
					if (isdigit(line1[i])) {
						free_memory.push_back(line1[i]);
					}
				}

				//cout <<endl <<free_memory <<endl;
			} else {
				break;
			}

			line_counter++;
		}	
	}

	total_memory = getTotalMemory();

	free_mem = atoi(free_memory.c_str()) / 1024;

	utilization = 1 - ((double)free_mem / (double)total_memory);

	input.close();

	return utilization;
}

void prepareSystemSpecDS(void)
{
	specs = getSystemSpecs();
}

