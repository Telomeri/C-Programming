#include <stdio.h>	
#include <stdlib.h>
#include <string.h>
#include <sys/file.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include "clear_make.h"




int main(void)
{
	//defining the stuff we need
	FILE *log;
	pid_t pid;
	char file_name[50];
	//writing a log file, that is why the fprintf:s are here.
	log = fopen("clean_files.log", "a+");
	fprintf(log,"Starting the program \n" );
	//takes imput of the wanted file name	
	printf("Enter a file name you want cleaned \nExit by entering exit as a filename or press CTRL + C \n" );
	fprintf(log,"Enter a file name you want cleaned \nExit by entering exit as a filename or press CTRL + C \n" );
	signal(SIGINT, sigshutdown);
	fprintf(log,"scanning file name \n" );
	scanf("%s", file_name);
	fprintf(log,"entered file name is %s \n", file_name );
	while (strcmp(file_name, "exit") != 0) {
		//creates a child process to clean the file
		pid = fork();
		if (pid == 0) {
			//if file clean is success, exit child and print file cleaned
			if (clean_file(file_name) == 1) {
				printf("file cleaned \n ");
				fprintf(log,"file cleaned \n ");
				exit(0);
			}
			else {
				printf("cleaning file was not succesful\n");
				fprintf(log,"cleaning file was not succesful\n ");
				exit(0);
			}
		}
		//incase child creation fails
		else if (pid < 0) {
			perror("creating a child process failed");
			fprintf(log,"error creating child process \n ");
		}
		else {
		//waits for child (atleast should) and takes another file to be cleaned
			pid = wait(NULL);
			printf( "Enter another file name you want cleaned\n");
			fprintf(log,"Enter another file name you want cleaned\n");
			signal(SIGINT, sigshutdown);
			fprintf(log,"entered file name is %s \n", file_name );
			scanf("%s", file_name);
		}
	}
	//job done
	printf("exiting the program succesfully using exit command\n");
	fprintf(log,"exiting the program succesfully using exit command\n");
	return 0;
}
