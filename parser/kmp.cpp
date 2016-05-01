#include <iostream>
#include <cstdlib>
#include <cstring>
#include <vector>

using namespace std;

int kmpSearch(string, string);
int kmpSearch(string, string, vector<string>&, vector<int>&, int&);
int kmpSearch(string, string, vector<string>&, int);
int kmpSearch(string, string, int&);
void computeLongestPrefix(string, int, int*);

extern int KMP_FLAG;

int kmpSearch(string text, string pattern)
{
        int counter_for_line;
        int length_of_text = text.length();
        int length_of_pattern = pattern.length();

        int *longest_proper_prefix = (int*) malloc(sizeof(int) * length_of_pattern);

        int index_pattern;
        int index_text;

        computeLongestPrefix(pattern, length_of_pattern, longest_proper_prefix);

        index_text = 0;
        index_pattern = 0;
        counter_for_line = 0;

        while (index_text < length_of_text) {
                if (pattern[index_pattern] == text[index_text]) {
                        index_pattern++;
                        index_text++;
                }

                if (index_pattern == length_of_pattern) {
                        //cout <<"Pattern found at index " <<index_text - index_pattern <<endl;
                        counter_for_line++;

                        if ((KMP_FLAG == 0) || (KMP_FLAG == 1))
                                break;

                        index_pattern = longest_proper_prefix[index_pattern - 1];
                } else if ((index_text < length_of_text) && (pattern[index_pattern] != text[index_text])) {
                        if (index_pattern != 0)
                                index_pattern = longest_proper_prefix[index_pattern - 1];
                        else
                                index_text = index_text + 1;
                }
        }

        free(longest_proper_prefix);

        return counter_for_line;
}

int kmpSearch(string text, string pattern, int &counter)
{
        int match;
        int length_of_text = text.length();
        int length_of_pattern = pattern.length();

        int *longest_proper_prefix = (int*) malloc(sizeof(int) * length_of_pattern);

        int index_pattern;
        int index_text;
        int index_module_name;

        string module;

        computeLongestPrefix(pattern, length_of_pattern, longest_proper_prefix);

        index_text = 0;
        index_pattern = 0;
        match = 0;

        while (index_text < length_of_text) {
                if (pattern[index_pattern] == text[index_text]) {
                        index_pattern++;
                        index_text++;
                }

                if (index_pattern == length_of_pattern) {
                        index_module_name = index_text - index_pattern;

                        cout <<index_module_name <<endl;

                        counter++;

                        match = 1;

                        index_pattern = longest_proper_prefix[index_pattern - 1];
                } else if ((index_text < length_of_text) && (pattern[index_pattern] != text[index_text])) {
                        if (index_pattern != 0)
                                index_pattern = longest_proper_prefix[index_pattern - 1];
                        else
                                index_text = index_text + 1;
                }
        }

        free(longest_proper_prefix);

        return match;
}

int kmpSearch(string text, string pattern, vector<string> &depends_module_names, int i)
{
        int iterator;
        int length_of_text;
        int length_of_pattern;
        int *longest_proper_prefix;
        int counter_for_line;
        int match;
        int index_pattern;
        int index_text;
        int index_module_name;
        int iterator2;

        length_of_text = text.length();
        length_of_pattern = pattern.length();
        counter_for_line = 0;
        match = 0;
        index_text = 0;
        index_pattern = 0;
        index_module_name = 0;

        longest_proper_prefix = (int*) malloc(sizeof(int) * length_of_pattern);

        while (index_text < length_of_text) {
                if (pattern[index_pattern] == text[index_text]) {
                        index_pattern++;
                        index_text++;
                }
                if (index_pattern == length_of_pattern) {
                        index_module_name = index_text - index_pattern;
                        counter_for_line++;

                        if (counter_for_line == 1) {
                                for (iterator2 = index_module_name; iterator2 < text.size(); iterator2++) {
                                        if (text[iterator2] == '>')
                                                break;

                                        depends_module_names[counter_for_line - 1].push_back(text[iterator2]);
                                }
                        } else if (counter_for_line > 1) {
                                for (iterator2 = index_module_name; iterator2 < text.size(); iterator2++) {
                                        if (text[iterator2] == ',')
                                                break;

                                        depends_module_names[counter_for_line - 1].push_back(text[iterator2]);
                                }
                        }

                        /*
                        for (int index = index_module_name; index < text.size(); index++) {
                                if (text[index] == '>')
                                        break;

                                module_name.push_back(text[index]);
                        }

                        modules.push_back(module_name);
                        */

                        match = 1;

                        index_pattern = longest_proper_prefix[index_pattern - 1];
                } else if ((index_text < length_of_text) && (pattern[index_pattern] != text[index_text])) {
                        if (index_pattern != 0)
                                index_pattern = longest_proper_prefix[index_pattern - 1];
                        else
                                index_text = index_text + 1;
                }
        }

        free(longest_proper_prefix);

        return match;
}

int kmpSearch(string text, string pattern, vector<string> &modules, vector<int> &level_of_modules, int &counter)
{
        int length_of_text = text.length();
        int length_of_pattern = pattern.length();

        int *longest_proper_prefix = (int*) malloc(sizeof(int) * length_of_pattern);

        int index_pattern;
        int index_text;
        int index_module_name;
        int match;
        string module_name;

        computeLongestPrefix(pattern, length_of_pattern, longest_proper_prefix);

        index_text = 0;
        index_pattern = 0;
        index_module_name = 0;
        match = 0;

        while (index_text < length_of_text) {
                if (pattern[index_pattern] == text[index_text]) {
                        index_pattern++;
                        index_text++;
                }
                if (index_pattern == length_of_pattern) {
                        index_module_name = index_text - index_pattern;

                        for (int index = index_module_name; index < text.size(); index++) {
                                if (text[index] == '>')
                                        break;
                                if (text[index] != ' ')
                                        module_name.push_back(text[index]);
                        }

                        modules.push_back(module_name);

                        level_of_modules.push_back(counter);

                        match = 1;

                        break;

                        index_pattern = longest_proper_prefix[index_pattern - 1];
                } else if ((index_text < length_of_text) && (pattern[index_pattern] != text[index_text])) {
                        if (index_pattern != 0)
                                index_pattern = longest_proper_prefix[index_pattern - 1];
                        else
                                index_text = index_text + 1;
                }
        }

        free(longest_proper_prefix);

        if (match == 0)
                counter++;

        return match;
}

void computeLongestPrefix(string pattern, int length_of_pattern, int *longest_proper_prefix)
{
        int length_of_longest_match;
        int index;

        longest_proper_prefix[0] = 0;

        index = 1;
        length_of_longest_match = 0;

        while (index < length_of_pattern) {
                if (pattern[index] == pattern[length_of_longest_match]) {
                        length_of_longest_match++;
                        longest_proper_prefix[index] = length_of_longest_match;
                        index++;
                } else {
                        if (length_of_longest_match != 0)
                                length_of_longest_match = longest_proper_prefix[length_of_longest_match - 1];
                        else {
                                longest_proper_prefix[index] = 0;
                                index++;
                        }
                }
        }
}

