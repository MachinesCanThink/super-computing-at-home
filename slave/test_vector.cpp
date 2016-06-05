#include <iostream>
#include <vector>

using namespace std;

int main()
{
	vector<int> test;

	test.push_back(5);
	test.push_back(6);
	test.push_back(7);
	test.push_back(8);

	int element = test.at(test.size() - 1);
	cout <<element <<endl;
	test.pop_back();
	element = test.at(test.size() - 1);


	cout <<element <<endl;

	return 0;
}