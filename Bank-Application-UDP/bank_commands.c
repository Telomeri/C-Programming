#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <unistd.h> 

struct stat st = {0};

//takes bank commands and handles writing to files
//If there is no current bank account for account number, opens one and sets balance to 1000
int process_command(int account_num, char *command) {
    int balance, withdrawn;
    FILE * account_file, * holder_file;
    char direk[30];
    char buffer[30];
    sprintf(buffer, "%i",account_num);
    strcat(strcat(strcpy(direk, "bank_accounts/"),buffer),".txt");
    if (stat("bank_accounts", &st) == -1) {
        mkdir("bank_accounts", 0777);
    }
    account_file = fopen(direk, "r+");
    if (account_file == NULL) {
        printf("Given account %i does not exist, creating it with balance of 1000 \n", account_num);
        account_file = fopen(direk, "w+");
        fprintf(account_file,"%i",1000);
        rewind(account_file);
    }
    flock(fileno(account_file), 1);
    switch (command[0]) {
        case 'l' : 
        //get the balance of your own account
            if (fscanf(account_file,"%i", &balance) == 1) {
                printf("The account %i balance is %d \n",account_num,balance);
            }
            else {
                printf("The account %i balance could not be read\n",account_num);
                fclose(account_file);
                flock(fileno(account_file), 8);
                return 0;
            }
            break;
        case 'w' :
        //withdraw from your own account
            if (fscanf(account_file,"%i", &balance) == 1) {
                rewind(account_file);
                sscanf(command, "%s %i", buffer, &withdrawn);
                printf("The account %i balance was %i before withdraw, ",account_num,balance);
                if (balance < withdrawn) {
                    printf("the operation was unsuccessful because of low balance \n");
                    flock(fileno(account_file), 8);
                    fclose(account_file);
                    return 0;
                } 
                balance = balance - withdrawn;
                fclose(account_file);
                holder_file = fopen(direk, "w");
                fprintf(holder_file,"%i",balance);
                printf("amount withdrawn %i, current balance %i \n", withdrawn, balance);
                flock(fileno(account_file), 8);
                fclose(holder_file);
                return 1;
            }
            else {
                printf("Withdrawing money from account %i unsuccessful \n",account_num);
                flock(fileno(account_file), 8);
                fclose(account_file);
                return 1;
            }
            break;

            //was going to be code for bank transfer but ran out of time
        /*case 't':
        //transfer money from your account to someone else
            printf("transfer money\n");
            if (fscanf(account_file,"%i", &balance) == 1) {
                rewind(account_file);
                sscanf(command, "%s %i %i", buffer, &withdrawn, deposit_acc);
                printf("The account %i balance was %i before deposit to acc, ",account_num,balance, deposit_acc);
                if (balance < withdrawn) {
                    printf("the operation was unsuccessful because of low balance \n");
                    flock(fileno(account_file), 8);
                    fclose(account_file);
                    return 0;
                } 
                balance = balance - withdrawn;
                fclose(account_file);
                holder_file = fopen(direk, "w");
                fprintf(holder_file,"%i",balance);
                printf("amount withdrawn %i, current balance %i \n", withdrawn, balance);
                flock(fileno(account_file), 8);

                memset(buffer, '\0', sizeof(buffer)); 
                memset(direk, '\0', sizeof(direk)); 
                sprintf(buffer, "%i",deposit_acc);
                strcat(strcat(strcpy(direk, "bank_accounts/"),buffer),".txt");
                if (transfer_file = fopen(direk, "r")){
                    
                }
                transfer_file = fopen(direk, "w");
                fprintf(holder_file,"%i",withdrawn);
                fclose(holder_file);
                return 1;
            }
            else {
                printf("Withdrawing money from account %i unsuccessful \n",account_num);
                fclose(account_file);
                return 1;
            }
            break;*/
        case 'd':
            //same as withdraw but deposist, could have been made seperate func and passed parameter 0 or 1 signaling which process should be done.
            if (fscanf(account_file,"%i", &balance) == 1) {
                rewind(account_file);
                sscanf(command, "%s %i", buffer, &withdrawn);
                printf("The account %i balance was %i before deposit, ",account_num,balance);
                balance = balance + withdrawn;
                fclose(account_file);
                holder_file = fopen(direk, "w");
                fprintf(holder_file,"%i",balance);
                printf("amount deposited %i, current balance %i \n", withdrawn, balance);
                flock(fileno(account_file), 8);
                fclose(holder_file);
                return 1;
            }
            else {
                printf("Depositing money to account %i unsuccessful \n",account_num);
                flock(fileno(account_file), 8);
                fclose(account_file);
                return 1;
            }
        case 'e': 
            printf("User %i might be leaving...\n",account_num);
            break;
        default:
            //if the command does not work
            flock(fileno(account_file), 8);
            fclose(account_file);
            printf("given command is not a bank command: %s", command);
            return 0;
    }
    //makes it here if balance check is successful
    fclose(account_file);
    flock(fileno(account_file), 8);
    return 1;
}