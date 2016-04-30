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
extern int kmpSearch(string, string, vector<string>&, vector<int>&, int&);
extern int kmpSearch(string, string, vector<string>&, int);
extern void initVector(int, vector<int>);
extern int fillVector(vector< vector<string> >, vector<int>, vector<string>, map<string, int>, int*);

void readFileToString(char*, vector<string>&);
int getLevelCount(vector<string>&);
int getModuleCount(vector<string>&);
void getModuleNames(vector<string>&, vector<string>&, vector<int>&);
void getNumberOfDepends(vector<string>&, int*, int);
void getDependsModuleNames(vector<string>&, vector< vector<string> >&);
void getNumberOfModulesInLevels(vector<int>&, vector<int>&);
int checkForErrors(vector<string>&);

int main(int argc, char *argv[])
{
        int level_count;
        int module_count;
        int iterator;
        int iterator2;
        int status;

        vector<string> lines_from_file;
        vector<string> module_names;
        vector<int> level_of_modules;
        vector<int> num_modules_in_level;
        // A 2D vector.
        vector< vector<string> > depend_modules;

        map<int, string> map_module_names;
        map<string, int> map_module_names_version2;

        int *num_deps;

        readFileToString(argv[1], lines_from_file);

        level_count = getLevelCount(lines_from_file);
        module_count = getModuleCount(lines_from_file);

        //DEBUG CODE
        cout <<"Levels = " <<level_count <<endl;
        cout <<"Total number of modules = " <<module_count <<endl;

        num_deps = (int*) calloc(module_count, sizeof(int));

        getModuleNames(lines_from_file, module_names, level_of_modules);

        for (iterator = 0; iterator < module_names.size(); iterator++) {
            map_module_names[iterator] = module_names[iterator];
        	map_module_names_version2[module_names[iterator]] = iterator;
        }

        // DEBUG CODE
        for (iterator = 0; iterator < map_module_names.size(); iterator++) {
               cout <<"Module: " <<map_module_names[iterator] <<":level = " <<level_of_modules[iterator] <<endl;
        }

        //cout <<endl <<endl;

        // Size the first dimention of the 2D vector of strings.
        depend_modules.resize(module_count);

        getNumberOfDepends(lines_from_file, num_deps, module_count + level_count);

        // Size the second dimention of the 2D vector of strings.
        for (iterator = 0; iterator < module_count; iterator++) {
        	depend_modules[iterator].resize(num_deps[iterator] + 1);
        }

        // DEBUG CODE
        for (iterator = 0; iterator < module_count; iterator++) {
                cout <<"Module: " <<map_module_names[iterator] << ":: Number of Dependencies: " <<num_deps[iterator] <<endl;
        }

        getDependsModuleNames(lines_from_file, depend_modules);

        cout <<endl;

        // DEBUG CODE
        for (iterator = 0; iterator < depend_modules.size(); iterator++) {
        	for (iterator2 = 0; iterator2 < depend_modules[iterator].size(); iterator2++) {
        		if (iterator2 == 0)
        			cout <<"Module: " <<depend_modules[iterator][iterator2] <<" --> ";
        		else
        			cout <<depend_modules[iterator][iterator2] <<"   ";
        	}

        	cout <<endl;
        }

        getNumberOfModulesInLevels(level_of_modules, num_modules_in_level);

        // DEBUG CODE
        cout <<"The number of modules in each level --" <<endl;
        for (iterator = 0; iterator < num_modules_in_level.size(); iterator++)
        	cout <<"Level " <<iterator << " : " <<num_modules_in_level[iterator] <<endl;

        // Initialize the vector.
        initVector(level_count, num_modules_in_level);

        // Add contents to the vector.
        // The below code has been commented because it isn't finished yet. work in progress.
        //status = fillVector(depend_modules, level_of_modules, module_names, map_module_names_version2, num_deps);

        cout <<status <<endl;

        return 0;
}

void getNumberOfModulesInLevels(vector<int> &level_of_modules, vector<int> &num_modules_in_level)
{
	int iterator;
	int iterator2;
	int counter;

	counter = 0;
	iterator2 = 0;


	for (iterator = 0; iterator < level_of_modules.size(); iterator++) {
		if (iterator2 == level_of_modules[iterator]) {
			counter++;
		} else if (level_of_modules[iterator] > iterator2) {
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
        pattern = "module";

        for (iterator = 0; iterator < lines_from_file.size(); iterator++) {
                counter = counter + kmpSearch(lines_from_file[iterator], pattern);
        }

        return counter;
}

void getModuleNames(vector<string> &lines_from_file, vector<string> &module_names, vector<int> &level_of_modules)
{
        string pattern;
        int iterator;
        int counter;

        //setting the KMP_FLAG to get the number of modules.
        KMP_FLAG = KMP_MODULENAMES;

        pattern = "module";

        counter = -1;

        for (iterator = 0; iterator < lines_from_file.size(); iterator++) {
                if (kmpSearch(lines_from_file[iterator], pattern, module_names, level_of_modules, counter)) {
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
                counter = counter + kmpSearch(lines_from_file[iterator], pattern);
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
