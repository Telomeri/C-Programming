#include <stdio.h>	
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>

void sigshutdown(int sig) {
	//shutting down with a signal wont be added to the log file
	printf("shutting down using Signal CTRL + C\n");
	exit(1);
}
