#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <cstring>
#include <cstdlib>

using namespace std;

typedef struct node
{
	int module_id;
	int num_dependencies;
	int level;

	vector<int> dep_id;

	//string module_name;
} node;

extern vector<int> getDependentModuleIds(int);

map<string, int> hack_map;

void declareVector(int, vector<int>);
void printVector(void);
int fillVector(vector< vector<string> >, vector<int>, vector<string>, map<string, int>, map<string, int>);

vector<int> num_modules_in_levels;
int num_levels;

vector< vector<node> >  task_vector;

string getMoudleNamesFromIds()

void declareVector(int level_count, vector<int> modules_in_level) 
{
	int iterator;
	int counter;

	counter = 0;

	num_modules_in_levels = modules_in_level;
	num_levels = level_count;

	task_vector.resize(level_count);

	for (iterator = 0; iterator < level_count; iterator++) {
		task_vector[iterator].resize(modules_in_level[iterator]);
	}
}

int fillVector(vector< vector<string> > depend_modules, vector<int> level_details, vector<string> module_names, map<string, int> module_name_and_id, map<string, int> num_deps)
{
	int iterator;
	int iterator2;
	int counter1;

	counter1 = 0;

	for (iterator = 0; iterator < task_vector.size(); iterator++) {
		for (iterator2 = 0; iterator2 < task_vector[iterator].size(); iterator2++, counter1++) {
			task_vector[iterator][iterator2].module_id = module_name_and_id[module_names[counter1]];
			task_vector[iterator][iterator2].level = level_details[counter1];
			task_vector[iterator][iterator2].num_dependencies = num_deps[module_names[counter1]];
			task_vector[iterator][iterator2].dep_id = getDependentModuleIds(counter1);
		}
	}

	return 1;
}

void printVector(void) 
{
	int iterator;
	int iterator2;

	for (iterator = 0; iterator < task_vector.size(); iterator++) {
		for (iterator2 = 0; iterator2 < task_vector[iterator].size(); iterator2++) {
			cout <<"Module ID: " <<task_vector[iterator][iterator2].module_id <<endl;
			cout <<"Module level: " <<task_vector[iterator][iterator2].level <<endl;
			cout <<"Number of dependencies : " <<task_vector[iterator][iterator2].num_dependencies <<endl;

			cout <<"ID's of dependent modules : ";
			for (int i = 0; i < task_vector[iterator][iterator2].dep_id.size(); i++) {
				cout <<task_vector[iterator][iterator2].dep_id[i] <<"  ";
			}

			cout <<endl <<endl;
		}
	}
}