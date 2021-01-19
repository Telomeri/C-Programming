#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 
#include "bank_server.h"

#define LIMIT    255

int create_desk(int desknum, int PORT) { 
    char buffer[LIMIT]; 
    socklen_t len;
    int sockfd, n; 
    struct sockaddr_in servaddr, cliaddr; 
    //the socket part is from https://www.geeksforgeeks.org/udp-server-client-implementation-c/
    // Creating socket file descriptor 
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
        perror("socket creation failed"); 
        exit(EXIT_FAILURE); 
    } 
    memset(buffer, '\0', sizeof(buffer)); 
    memset(&servaddr, 0, sizeof(servaddr)); 
    memset(&cliaddr, 0, sizeof(cliaddr)); 
    // Filling server information 
    servaddr.sin_family    = AF_INET; // IPv4 
    servaddr.sin_port = htons(PORT); 
    servaddr.sin_addr.s_addr = INADDR_ANY; 

      
    // Bind the socket with the server address 
    if ( bind(sockfd, (const struct sockaddr *)&servaddr,  
            sizeof(servaddr)) < 0 ) 
    { 
        perror("bind failed"); 
        exit(EXIT_FAILURE); 
    } 

    //while loop takes commands until exit is given, then closes thread and waits for others to finish if there are active customers
    while(strcmp(buffer, "exit\n") != 0) {
        memset(buffer, '\0', sizeof(buffer)); 
        n = recvfrom(sockfd, (char *)buffer, LIMIT,  
                    MSG_WAITALL, ( struct sockaddr *) &cliaddr, 
                    &len); 
        buffer[n] = '\0';
        if (process_command(PORT, buffer) != 0) {
            printf("Request processed from desk %i from client number %i: %s", desknum,PORT, buffer); 
        }
        //used to be the send func, did not get it to work
        /*sendto(sockfd, (const char *)buffer, strlen(buffer),  
        MSG_CONFIRM, (const struct sockaddr *) &cliaddr, 
            len); */
    }
    printf("Customer exiting\n");
    return 0; 
} 
