#include <iostream>
#include <cstdlib>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/fs.h>
#include <syslog.h>

using namespace std;

extern int listenToPingRequestFromMaster(int&);
extern int readFromMaster(int, char*);
extern int writeToMaster(int, char*);

int main(int argc, char *argv[])
{
	int pid;

	int socket_fd;
	int original_socket;

	char response_message[64];
	char message[64];

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
		cout <<"ERROR: Cannot set the working dir to '/'. Slave failed to run." <<endl;

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

	/* And now, we have a daemon process running. more functionality coming here soon. */
	if ((socket_fd = listenToPingRequestFromMaster(original_socket)) > -1) {
		syslog(LOG_NOTICE, "master has contacted the slave.");



		if ((readFromMaster(socket_fd, message)) == -1) {
			syslog(LOG_NOTICE, "read failed so skipping write");
		} else {
			if ((writeToMaster(socket_fd, response_message)) == -1) {
				syslog(LOG_NOTICE, "write failed. so quitting.");
			} else {
				// Make an entry of the master's contact. Close the socket for now.
				close(socket_fd);
				close(original_socket);
			}
		}
	}

	syslog(LOG_NOTICE, "Slave daemon terminated. Bye bye!");

	return 0;
}
