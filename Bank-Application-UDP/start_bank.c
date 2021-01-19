#include <stdio.h>	
#include <stdlib.h>
#include <string.h>
#include <sys/file.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 
#include <sys/file.h>
#include <ctype.h>
#include <signal.h>

#include "bank_server.h"


int N_DESK;
int current_customers = 0;
int k = 0;
int i = 0;


static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cv;
pthread_t tid[255];


void sigshutdown(int sig) {
	//shutting down with a signal wont be added to the log file
	printf("Shutting down using Signal CTRL + C\n");
	exit(1);
}


void * receive_customer() {
    //client connects to port 8080, and gives a "customer number", which is the port where this thread and the client will communicate
    int sockfd, n; 
    struct sockaddr_in servaddr, cliaddr; 
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
        printf("fails here");
        perror("socket creation failed"); 
        exit(EXIT_FAILURE); 
    } 
    memset(&servaddr, 0, sizeof(servaddr)); 
    memset(&cliaddr, 0, sizeof(cliaddr)); 
      
    // Filling server information 
    servaddr.sin_family    = AF_INET; // IPv4 
    servaddr.sin_port = htons(8080); 
    servaddr.sin_addr.s_addr = INADDR_ANY; 
    if ( bind(sockfd, (const struct sockaddr *)&servaddr,  
            sizeof(servaddr)) < 0 ) 
    { 
        //tried to make this work and not crash the bank, but not successful..
        //so right now if two people with same customer number connect, server crashes
        //also crashes if same account joins twice a session, so all in all very spaghetti
        perror("Account joining failed"); 
        pthread_exit(NULL); 
    } 
    socklen_t len = sizeof(cliaddr);
    char buffer[LIMIT] = {0}; 
    char bank_statement[LIMIT+20] = {0};
    n = recvfrom(sockfd, (char *)buffer, LIMIT,  
                MSG_WAITALL, ( struct sockaddr *) &cliaddr, 
                &len); 
    buffer[n] = '\0';   
    printf("Request for desk received from client number %s \n", buffer); 
    strcpy(bank_statement,"Bank has received your request for desk\n");
    //The only backwards communication that works
    sendto(sockfd, (const char *)bank_statement, strlen(bank_statement),  
        MSG_CONFIRM, (const struct sockaddr *) &cliaddr, 
            len); 
    close(sockfd);
    current_customers++;
    pthread_mutex_unlock(&lock);
    FILE *log_f = fopen("bank_server.log", "a+");
    fprintf(log_f,"Opened a thread for account %i on desk %i \n",(int)strtol(buffer,NULL, 10),current_customers);
    fclose(log_f);
    //sends this to create desk
    create_desk(current_customers, (int)strtol(buffer,NULL, 10));
    log_f = fopen("bank_server.log", "a+");
    fprintf(log_f,"Exiting a thread for account %i on desk %i, current balance can be seen on its file \n",(int)strtol(buffer,NULL, 10),current_customers);
    fclose(log_f);
    return 0;
}

void * open_desk(void * arg){
    pthread_mutex_lock(&lock);
    receive_customer();
    return 0;
}
int main(int argc, char *argv[]) {
    if ( argc == 2) {
        if (atoi(argv[1]) == 0) {
            printf("The desk amount should be an integer \n");
            return 0;
        }
        if (atoi(argv[1]) < 1 || atoi(argv[1]) > 255) {
            printf("Please note that you should have 1-255 desks\n");
            return 0;
        }
    }
    else {
        printf("Please give the amount of desks as a second parameter (e.g. ./bank_connect 3) \n");
        return 0;
    }
    N_DESK = atoi(argv[1]);
    printf("Welcome to the server side of the bank!\nYou must be curious about how this works?\n");
    printf("Before you is a server side of the bank, where you will see what the client sends and what it does.\n");
    printf("Ideally we would also be able to send this info to clients.\n");
    printf("To exit the server side, press CTRL + C.\n");
    printf("You have chosen to use %i desks\n",atoi(argv[1]));
    FILE *log_f = fopen("bank_server.log", "a+");
    if (log_f == NULL) {
        printf("creating log failed");
    }
    fprintf(log_f, "Starting bank program, the amount of desk(s) used: %i\n", N_DESK);
    fclose(log_f);
    //the thread part, creates N amount of desks to wait for customers. When there are 0 current customers, syncs the threads.
    while(1) {
        signal(SIGINT, sigshutdown);
        for (; k < N_DESK; k++) {
            pthread_create(&(tid[k]), NULL, open_desk, NULL);
            i = 0;
            }
        for (; i <= N_DESK; i++) {
            pthread_join((tid[i]), NULL);
            k = 0;
            current_customers = 0;
            }
    }
    //this is never achieved, since the program is shut down with ctrl + C
    log_f = fopen("bank_server.log", "a+");
    fprintf(log_f, "Exiting server: \n");
    printf("Exiting server \n");
    fclose(log_f);
    pthread_exit(NULL);
    return 0;
}