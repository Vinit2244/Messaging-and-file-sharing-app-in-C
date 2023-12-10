#ifndef _CLIENT_H_
#define _CLIENT_H_

#include "../common_headers/headers.h"

extern char* my_ip;
extern int credentials_saved;
extern char clear[10];

int send_signup_req();
int send_signin_req();
char* get_my_ip();
size_t write_callback(void *contents, size_t size, size_t nmemb, char **result);
void ask_to_save_credentials();
int send_signout_req();
int send_delete_account_req();
void handleCtrlC(int signum);

void* receive_client_data(void* args);

extern struct sockaddr_in client_address;       // IPv4 address struct for TCP communication between ss and nfs (requests)
extern int client_sock_fd;

#endif