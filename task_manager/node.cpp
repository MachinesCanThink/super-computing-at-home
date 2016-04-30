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

	string module_name;
} node;

void initVector(int, vector<int>);
int fillVector(vector< vector<string> >, vector<int>, vector<string>, map<string, int>, int[]);

vector<int> num_modules_in_levels;
int num_levels;

vector< vector<node> >  task_vector;

void initVector(int level_count, vector<int> modules_in_level) 
{
	int iterator;
	int counter;

	counter = 0;

	num_modules_in_levels = modules_in_level;
	num_levels = level_count;

	task_vector.resize(level_count);

	for (iterator = 0; iterator < level_count; iterator++) {
		task_vector.resize(modules_in_level[counter]);
		counter++;
	}
}

int fillVector(vector< vector<string> > depend_modules, vector<int> level_details, vector<string> module_names, map<string, int> module_name_and_id, int num_deps[])
{
	int iterator;
	int iterator2;
	int counter;
	int counter1;

	counter = 0;
	counter1 = 0;

	for (iterator = 0; iterator < num_levels; iterator++) {
		for (iterator2 = 0; iterator2 < num_modules_in_levels[counter]; iterator2++) {
			task_vector[iterator][iterator2].module_name = module_names[counter1];
			task_vector[iterator][iterator2].module_id = module_name_and_id[module_names[counter1]];
			task_vector[iterator][iterator2].level = level_details[counter1];
			task_vector[iterator][iterator2].num_dependencies = num_deps[counter1];
			
			/*for (int i = 0; i < depend_modules.size(); i++) {
				for (int j = 1; j < depend_modules[i].size(); j++) {
					task_vector[iterator][iterator2].dep_id[j] = module_name_and_id[depend_modules[i][j]];
				}
			}*/
		}
	}

	return 1;
}