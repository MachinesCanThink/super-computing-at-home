#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <fstream>
#include <ctype.h>

using namespace std;

int getMemoryUtilization(void)
{
	ifstream input;

	int counter;
	int iterator;
	int iterator2;
	int total_memory;
	int free_memory;

	string line;
	string total_mem;
	string free_mem;

	input.open("/proc/meminfo", ifstream::in);
	counter = 0;

	if (input.is_open()) {
		while(getline(input, line)) {
			if (counter == 0) {
				counter++;
			} else if (counter == 1) {
				string line2 = line;

				for (iterator = 0, iterator2 = 0; iterator < line2.size(); iterator++) {
					if (isdigit(line2[iterator])) {
						free_mem[iterator2++] = line2[iterator];
					}
				}

				counter++;
			} else {
				break;
			}
		}	
	}

	free_memory = atoi(free_mem.c_str()) / 1024;

	cout <<"free memory: " <<free_memory <<endl;
}

int main(void) {
	getMemoryUtilization();

	return 0;
}