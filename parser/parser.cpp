#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <cstring>

#define KMP_LEVEL 0
#define KMP_MODULES 1
#define KMP_MODULENAMES 2

using namespace std;

int KMP_FLAG;

extern int kmpSearch(string, string);
extern int kmpSearch(string, string, vector<string>&);

void readFileToString(char*, vector<string>&);
int getLevelCount(vector<string>&);
int getModuleCount(vector<string>&);
void getModuleNames(vector<string>&, vector<string>&);

int main(int argc, char *argv[])
{
        int level_count;
        int module_count;
        int iterator;

        vector<string> lines_from_file;
        vector<string> module_names;

        map<int, string> map_module_names;

        readFileToString(argv[1], lines_from_file);

        level_count = getLevelCount(lines_from_file);
        module_count = getModuleCount(lines_from_file);

        cout <<"Levels = " <<level_count <<endl;
        cout <<"Total number of modules = " <<module_count <<endl;

        getModuleNames(lines_from_file, module_names);

        for (iterator = 0; iterator < module_names.size(); iterator++) {
        map_module_names[iterator] = module_names[iterator];
        }

        for (iterator = 0; iterator < map_module_names.size(); iterator++) {
               cout <<"Module: " << map_module_names[iterator] <<endl;
        }

        return 0;
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


void getModuleNames(vector<string> &lines_from_file, vector<string> &module_names)
{
        string pattern;
        int iterator;

        //setting the KMP_FLAG to get the number of modules.
        KMP_FLAG = KMP_MODULENAMES;

        pattern = "module";
        for (iterator = 0; iterator < lines_from_file.size(); iterator++) {
                if (kmpSearch(lines_from_file[iterator], pattern, module_names)) {
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
