typedef struct system_spec {
	bool is_hyperthreaded;
	int total_memory;
	int number_of_cores;
} system_spec;

typedef struct utils {
	double memory_utilization;
	double cpu_utilization;
} utilizations;