#ifndef BANKLIB
#define BANKLIB
#define LIMIT    255
#define SERVERPORT 8080
int create_desk(int desknum, int PORT);
int process_command(int account_num, char *command);
#endif
