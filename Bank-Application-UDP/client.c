#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 
#include "client.h"
  

//receives message from "server" 
void receive_message(int sockfd, struct sockaddr_in servaddr) {
    int n; 
    socklen_t len;
    char buffer[LIMIT] = {0}; 
    n = recvfrom(sockfd, (char *)buffer, LIMIT,  
                MSG_CONFIRM, (struct sockaddr *) &servaddr, 
                &len); 
    buffer[n] = '\0';
    printf("%s", buffer);
    }

//sends message to "server"
int send_message(int sockfd, struct sockaddr_in servaddr) {
    char * str1;
    str1 = malloc(sizeof(char)*LIMIT);
    char *message =  user_input(str1); 
    if (sendto(sockfd, (const char *)message, strlen(message), 
        MSG_CONFIRM, (const struct sockaddr *) &servaddr,  
            sizeof(servaddr)) == -1) {
                printf("error sending message to server");
            }
    if (strcmp(str1,"exit\n") == 0) {
        free(str1);
        printf("Shutting down\n");
        return 0;
    }
    free(str1);
    return 1;
}

int create_connection(int first_time, int PORT) {
    int sockfd; 
    int n = 1;
    char buffer[LIMIT]; 
    struct sockaddr_in servaddr; 
  
    //the socket part is from https://www.geeksforgeeks.org/udp-server-client-implementation-c/
    // Creating socket file descriptor 
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
        perror("socket creation failed"); 
        exit(EXIT_FAILURE); 
    } 
    memset(&servaddr, 0, sizeof(servaddr)); 
      
    // Filling server information 
    servaddr.sin_family = AF_INET; 
    
    servaddr.sin_addr.s_addr = INADDR_ANY;     
    if (first_time != 1) {
        servaddr.sin_port = htons(PORT); 
        while( n != 0 ) {
            n = send_message(sockfd, servaddr);
            //receive_message(sockfd,servaddr);
        }
    }
    else {
        servaddr.sin_port = htons(SERVERPORT); 
        sprintf(buffer, "%d",PORT);
        sendto(sockfd, (const char *)buffer, strlen(buffer), 
        MSG_CONFIRM, (const struct sockaddr *) &servaddr,  
            sizeof(servaddr)); 
        receive_message(sockfd,servaddr);
    }
    close(sockfd);
    return 0; 
}

int main() { 
    int account_num = 0;
    char *str1 = malloc(sizeof(char)*10);
    printf("Connecting to bank server\n");
    //the account num is like this because of ports, decided to make the numbers even
    while (account_num < 1300 || account_num > 8000) {
        printf("Enter your account number, it has to be between 1300 and 8000 \n");
        fgets(str1, LIMIT, stdin);
        account_num = atoi(str1);
    }
    free(str1);
    create_connection(1, account_num);
    create_connection(0, account_num);
} 
