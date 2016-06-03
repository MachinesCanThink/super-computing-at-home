#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <cstring>
#include <cstdlib>

#define KMP_LEVEL 0
#define KMP_MODULES 1
#define KMP_MODULENAMES 2
#define KMP_DEPENDS 3

using namespace std;

int KMP_FLAG;

extern int kmpSearch(string, string);
extern int kmpSearch(string, string, int&);
extern int kmpSearch(string, string, vector<string>&, vector<int>&, int&);
extern int kmpSearch(string, string, vector<string>&, int);

extern void declareVector(int, vector<int>);
extern void printVector(void);
extern int fillVector(vector< vector<string> >, vector<int>, vector<string>, map<string, int>, map<string, int>);

void readFileToString(char*, vector<string>&);
void createNameToIdMap(map<string, int>&, vector<string>);
void getModuleNamesAndLevels(vector<string>&, vector<string>&, vector<int>&);
void getDependsOnModulesNumber(vector<string>, vector<string>, map<string, int>&);
void getNumberOfDepends(vector<string>&, int*, int);
void getDependsModuleNames(vector<string>&, vector< vector<string> >&);
void getNumberOfModulesInEachLevel(vector<int>&, vector<int>&);
void fillidVector(vector< vector<string> >, map<string, int>, int, int[]);
int checkForErrors(vector<string>&);
int getLevelCount(vector<string>&);
int getModuleCount(vector<string>&);

vector< vector<int> > ids_of_dependent_modules;

int main(int argc, char *argv[])
{
        int level_count;
        int module_count;
        int iterator;
        int iterator2;
        int status;

        int *num_deps;

        vector<string> lines_from_file;
        vector<string> module_names;
        vector<int> module_level;
        vector<int> num_modules_in_level;

        // A 2D vector.
        vector< vector<string> > depend_modules;

        map<string, int> modulename_to_moduleid_map;
        map<string, int> number_of_dependencies;

        // copies file contents line by line into vector of strings
        readFileToString(argv[1], lines_from_file); 

        /* 
         * Step 1 of parser algorithm.
         * Get level_count. 
         */
        level_count = getLevelCount(lines_from_file);

        /* 
         * Stores the names of modules from .flow into a vector<string>.
         * Correspondingly, module_level is populated correspondingly, as per module_names.
         */
        getModuleNamesAndLevels(lines_from_file, module_names, module_level);

        /* Gets the number od modules in each level. */
        getNumberOfModulesInEachLevel(module_level, num_modules_in_level);

        /* 
         * Step 2 of parser algorithm.
         * Declare the df-graph.
         */
        declareVector(level_count, num_modules_in_level);

        /* 
         * Next step is to populate the df_graph. 
         * Create hashtable of module names and ids first.
         */
        createNameToIdMap(modulename_to_moduleid_map, module_names);

         /*
         * The function getDependsOnModulesNumber() obtains the number of modules a perticular
         * module x is dependent on.
         */
        getDependsOnModulesNumber(lines_from_file, module_names, number_of_dependencies);

        /*
         * The below block of code is for getting something. too bored and tired to explain.
         */
        module_count = getModuleCount(lines_from_file);

        num_deps = (int*) calloc(module_count, sizeof(int));

        // Size the first dimention of the 2D vector of strings.
        depend_modules.resize(module_count);

        getNumberOfDepends(lines_from_file, num_deps, module_count + level_count);

        // Size the second dimention of the 2D vector of strings.
        for (iterator = 0; iterator < module_count; iterator++) {
            depend_modules[iterator].resize(num_deps[iterator] + 1);
        }

        getDependsModuleNames(lines_from_file, depend_modules);

        // This is where i try to get the id's into the vector.
        fillidVector(depend_modules, modulename_to_moduleid_map, module_count, num_deps);

        fillVector(depend_modules, module_level, module_names, modulename_to_moduleid_map, number_of_dependencies);

        printVector();

        return 0;
}

void fillidVector(vector< vector<string> > dependent_module_names, map<string, int> name_to_id_map, int module_count, int num_deps[])
{
    int iterator;
    int iterator2;

    ids_of_dependent_modules.resize(module_count);

    for (iterator = 0; iterator < module_count; iterator++) {
        ids_of_dependent_modules[iterator].resize(num_deps[iterator]+1);
    }

    for (iterator = 0; iterator < ids_of_dependent_modules.size(); iterator++) {
        for (iterator2 = 0; iterator2 < ids_of_dependent_modules[iterator].size(); iterator2++) {
            ids_of_dependent_modules[iterator][iterator2] = name_to_id_map[dependent_module_names[iterator][iterator2]];
        }
    }
}

vector<int> getDependentModuleIds(int position)
{
    int iterator;

    vector<int> dependent_module_ids;

    for (iterator = 0; iterator < ids_of_dependent_modules[position].size(); iterator++) {
        if (ids_of_dependent_modules[position].size() == 1)
            dependent_module_ids.push_back(-1);
        else {
            if (iterator == 0)
                ;
            else {
                dependent_module_ids.push_back(ids_of_dependent_modules[position][iterator]);
            }
        }
    }

    return dependent_module_ids;
}

void getDependsOnModulesNumber(vector<string> lines_from_file, vector<string> module_names, map<string, int> &number_of_dependencies)
{
	int iterator;
	int iterator2;
	int counter = 0;

	for (iterator = 0; iterator < module_names.size(); iterator++) {
		for (iterator2 = 0; iterator2 < lines_from_file.size(); iterator2++) {
			if ((kmpSearch(lines_from_file[iterator2], module_names[iterator], counter))) {
				//cout <<"Module name: " <<module_names[iterator] <<" --> " <<counter <<endl;
			}
		}

		number_of_dependencies[module_names[iterator]] = counter - 1;
		counter = 0;
	}
}

void createNameToIdMap(map<string, int> &modulename_to_moduleid_map, vector<string> module_names) {
    for (int iterator = 0; iterator < module_names.size(); iterator++)  // hashtable 
        modulename_to_moduleid_map[module_names[iterator]] = iterator;
}

void getNumberOfModulesInEachLevel(vector<int> &module_level, vector<int> &num_modules_in_level)
{
	int iterator;
	int iterator2;
	int counter;

	counter = 0;
	iterator2 = 0;


	for (iterator = 0; iterator < module_level.size(); iterator++) {
		if (iterator2 == module_level[iterator]) {
			counter++;
		} else if (module_level[iterator] > iterator2) {
			iterator2++;
			num_modules_in_level.push_back(counter);
			counter = 1;
		}
	}

	num_modules_in_level.push_back(counter); 
}

void getDependsModuleNames(vector<string> &lines_from_file, vector< vector<string> > &depend_modules)
{
	int iterator;
	int iterator2;
	string pattern;

	pattern = "module";

	for (iterator = 0, iterator2 = 0; iterator < lines_from_file.size(); iterator++) {
		if ((kmpSearch(lines_from_file[iterator], pattern, depend_modules[iterator2], 1)))
			iterator2++;
	}
}

void getNumberOfDepends(vector<string> &lines_from_file, int num_depends[], int required_size)
{
        int iterator;
        int iterator2;
        int line_count;
        string pattern;

        int *temp_array;

        KMP_FLAG = KMP_DEPENDS;
        pattern = "module";
        line_count = 0;

        temp_array = (int*) calloc(required_size, sizeof(int));

        for (iterator = 0; iterator < lines_from_file.size(); iterator++) {
                line_count = kmpSearch(lines_from_file[iterator], pattern);
                if (line_count != 0)
                        temp_array[iterator] = line_count - 1;
                else
                        temp_array[iterator] = -1;
        }

        for (iterator = 0, iterator2 = 0; iterator < required_size; iterator++) {
                if (temp_array[iterator] != -1) {
                        num_depends[iterator2] = temp_array[iterator];
                        iterator2++;
                }
        }

        free(temp_array);
}

int getModuleCount(vector<string> &lines_from_file)
{
        string pattern;
        int counter;
        int iterator;

        //setting the KMP_FLAG to get the number of modules.
        KMP_FLAG = KMP_MODULES;

        counter = 0;
        pattern = "module"; // restricting module naming?

        for (iterator = 0; iterator < lines_from_file.size(); iterator++) {
                counter = counter + kmpSearch(lines_from_file[iterator], pattern);
        }

        return counter;
}

void getModuleNamesAndLevels(vector<string> &lines_from_file, vector<string> &module_names, vector<int> &module_level)
{
        string pattern;
        int iterator;
        int counter;

        //setting the KMP_FLAG to get the number of modules.
        KMP_FLAG = KMP_MODULENAMES;

        pattern = "module";

        counter = -1;

        for (iterator = 0; iterator < lines_from_file.size(); iterator++) {
                if (kmpSearch(lines_from_file[iterator], pattern, module_names, module_level, counter)) {
                        ;
                }
        }
}

int getLevelCount(vector<string> &lines_from_file)
{
        int counter;
        int iterator;

        // setting the KMP_FLAG to get the number of levels.
        KMP_FLAG = KMP_LEVEL;

        string pattern = "level";
        counter = 0;

        for (iterator = 0; iterator < lines_from_file.size(); iterator++) {
                counter += kmpSearch(lines_from_file[iterator], pattern);
        }

        return counter;
}

void readFileToString(char *file_name, vector<string> &lines_from_file)
{
        string line;
        ifstream input;

        input.open(file_name, ifstream::in);

        if (input.is_open()) {
                while (getline(input, line)) {
                        lines_from_file.push_back(line);
                }
        }
}

/*
int checkForErrors(vector<string> &lines_from_file)
{
	int iterator;
	regex levelcheckpattern("(level)[0-9]+:");
	regex multidependentmodule("[A-Za-z0-9]+>([A-Za-z0-9]+,)+");
	regex singledependentmodule("[A-Za-z0-9]+");
	for(iterator = 0; iterator < lines_from_file.size(); iterator++){
		if(regex_match(lines_from_file[iterator],multidependentmodule)){
		}
		else if(regex_match(lines_from_file[iterator],singledependentmodule)){
		}
		else{
			if(regex_match(lines_from_file[iterator],levelcheckpattern)){
			}
			else{
				cout<<"error in the file";
				return 0;                               //On failure//
			}
		}
	}
return 1;                                                               //On Success//
}*/
