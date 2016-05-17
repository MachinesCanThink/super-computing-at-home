/*
 * This program contains the functions and the data structures needed for 
 * storing the jobs and results and getting access to them.
 */

#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>

typedef struct struct_result {
	FILE *result;
	double exec_time;
	bool success;
	string result_file_name;
	string module_name;
} result;

typedef struct payload {
	FILE *module;
	bool success;
	int number_of_instructions;
	double exec_time;
	string module_name;
} payload;

typedef struct job_queue {
	FILE *module;
	string module_name;
} job;

vector<job> job_queue;
vector<result> result_queue;

// Functions realted to the JOB QUEUE.
bool addJobToQueue(job);
job dequeue(void);

// Funtions related to the RESULT QUEUE.
bool addResultToQueue(result);
result dequeueResult(void);

int addJobToQueue(job new_job)
{
	bool status;

	job_queue.push_back(new_job);

	status  = 1;

	return status;	
}

job dequeue(void)
{
	job new_job;

	new_job = job_queue.at(job_queue.size() - 1);

	job_queue.pop_back();

	return new_job;
}

int addResultToQueue(result new_result)
{
	bool status;

	result_queue.push_back(new_result);

	status = 1;

	return status;
}

result dequeueResult(void)
{
	result new_result;

	new_result = result_queue.at(result_queue.size() - 1);

	result_queue.pop_back();

	return new_result;
}









