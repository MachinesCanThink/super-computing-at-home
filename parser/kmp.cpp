#include <iostream>
#include <cstdlib>
#include <cstring>
#include <vector>

using namespace std;

int kmpSearch(string, string);
void computeLongestPrefix(string, int, int*);

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
