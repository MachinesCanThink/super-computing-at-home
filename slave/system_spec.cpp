#include <iostream>
#include <string>
#include <cstdlib>
#include <fstream>
#include <ctype.h>
#include <syslog.h>
#include <thread>

#include "system_spec.h"

using namespace std;

system_spec getSystemSpecs();
int getNumberOfCores(system_spec&);
int getHyperthreadingInfo(system_spec&);
int getTotalMemory(void);
int getNumberOfPhysicalCores(void);
void initializeSpecStruct(system_spec&);


void initializeSpecStruct(system_spec &spec)
{
	spec.is_hyperthreaded = 0;
	spec.number_of_cores = -1;
	spec.total_memory = -1;
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

	string file_name;
	string line;
	string numbers;

	file_name;

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

	return 1;
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

int main(int argc, char *argv[]) 
{
	system_spec spec;

	spec = getSystemSpecs();

	cout <<spec.is_hyperthreaded <<endl <<spec.total_memory <<endl <<spec.number_of_cores <<endl;

	return 0;
}