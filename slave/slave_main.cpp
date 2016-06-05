#include <iostream>
#include <cstdlib>
#include <vector>
#include <map>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/fs.h>
#include <syslog.h>

using namespace std;

extern int createConnectionWithMaster(int&, int);
extern int readFromMaster(int, char*);
extern int writeToMaster(int, char*);
extern void prepareSystemSpecDS(void);
extern int getStaticParams(void);

int function_count;
int command_type;
int unique_number;
int latest_module_id;
vector<string> function_names_from_cmd;
map<string, int> fname_to_int_map;

int main(int argc, char **argv)
{
	int pid;

	int socket_fd;
	int original_socket;

	char response_message[64];
	char message[64];

	int port_number;

	string p_number(argv[1]);

	if ((pid = fork()) < 0) {
		cout <<"ERROR: error while forking. Slave failed to run." <<endl;

		return -1;
	} 
	
	// Kill the parent process because I wants independence. *evil laugh* 
	if (pid != 0) {
		exit(EXIT_SUCCESS);
	}

	// Set the forked process as the session leader.
	if (setsid() == -1) {
		cout <<"ERROR: Cannot create new session and process group. Slave failed to run." <<endl;

		return -1;
	}

	// Change the working directory to / 
	if (chdir("/") == -1) {
		cout <<"ERROR: Cannot set the working dir to '/'. Slave failed to runn." <<endl;

		return -1;
	}

	// Close all open fd's inherited from the parent process.
	for (int i = 0; i < getdtablesize(); i++) {
		close(i);
	}

	// reditec the fd's 0, 1 and 2 to /dev/null because the process doesn't have a tty of it's own.
	open("/dev/null", O_RDWR);
	dup(0);
	dup(0);

	// Create a log file so that whatever happens here, gets recorded. No tty remember? 
	openlog("slavedaemonprocess", LOG_PID, LOG_DAEMON);
	syslog(LOG_NOTICE, "Slave daemon started.");

	/* And now, we have a daemon process running. 
	 * Initial connection and response by the salve 
	 */

	 port_number = atoi(p_number.c_str());

	if ((socket_fd = createConnectionWithMaster(original_socket, port_number)) > -1) {
		syslog(LOG_NOTICE, "master has contacted the slave.");

		if ((readFromMaster(socket_fd, message)) == -1) {
			syslog(LOG_NOTICE, "read failed so skipping write");
		} else {
			// init the systemSpacDS.
			prepareSystemSpecDS();

			parseCommandFromMaster(message);

			bzero(response_message, 64);

    		strcpy(response_message, "CHECK-BACK");

			if ((writeToMaster(socket_fd, response_message)) == -1) {
				syslog(LOG_NOTICE, "write failed. so quitting.");
			} else {
				// TODO: Make an entry of the master's contact. 
				//Close the socket for now.
				close(socket_fd);
				close(original_socket);
			}
		}
	}

	// Store the information about the system so that it can be sent to the master on request.
	/* 
	 * After this function call the spec DS will have the following information -
	 * Hyperthreading technology (yes|no)
	 * Number of cores on the CPU (Logical cores)
	 * Total memory on the system (in MBs)
	 */
	

	//getStaticParams();

	/* 
	 * Now we listen to the master for any command and exectute the command.
	 * We use a request-reponse model here for communication.
	 * The request packet is parsed, required data collected and is sent back to the master 
	 * in a response packet.
	 */
	 /*
	 if ((socket_fd = createConnectionWithMaster(original_socket)) > -1) {
	 	syslog(LOG_NOTICE, "Master has contacted the slave, again");

	 	if ((readFromMaster(socket_fd, message)) == -1) {
	 		syslog(LOG_NOTICE, "Read failed when listening for commands");
	 	} else {
	 		if ((parseCommandFromMaster(message)) == -1) {
	 			syslog(LOG_NOTICE, "Invalid command");
	 		}
	 	}
	 }
	 */

	syslog(LOG_NOTICE, "Slave daemon terminated. Bye bye!");

	return 0;
}
