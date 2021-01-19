#include <stdio.h>
#include <stdlib.h>
#include "client.h"

char* user_input(char *str1) {
    printf("Enter the bank statement you want to make: \n");
    fgets(str1, LIMIT, stdin);
    return str1;
}
