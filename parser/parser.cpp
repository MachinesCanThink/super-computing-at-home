#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstring>

using namespace std;

extern int kmpSearch(string, string);

void readFileToString(char*, vector<string>&);
int getLevelCount(vector<string> &lines_from_file);
int getModuleCount(vector<string>&);

int main(int argc, char *argv[])
{
        int level_count;
        vector<string> lines_from_file;

        readFileToString(argv[1], lines_from_file);

        level_count = getLevelCount(lines_from_file);
        module_count = getModuleCount(lines_from_file);

        cout <<"Levels = " <<level_count <<endl;
        cout <<"Total number of modules = " <<module_count <<endl;


        return 0;
}

int getModuleCount(vector<string> &lines_from_file)
{
        string pattern;
        int counter;
        int iterator;

        //setting the KMP_FLAG to get the number of modules.
        KMP_FLAG = 1;

        counter = 0;
        pattern = "module";

        for (iterator = 0; iterator < lines_from_file.size(); iterator++) {
                counter = counter + kmpSearch(lines_from_file[iterator], pattern);
        }

        return counter;
}


int getLevelCount(vector<string> &lines_from_file)
{
        int counter;
        int iterator;

        // setting the KMP_FLAG to get the number of levels.
        KMP_FLAG = 0;

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
