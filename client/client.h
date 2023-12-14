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
void print_menu();
int check_if_user_is_online(const int socket_fd, const int data_type, const char* to_username);
void menu();
void ask_signin_or_signup();
void send_file(char* file_path, int socket_fd, char* to_username, int flag, int end_flag);
void receive_data(int sock_fd, st_request recvd_data, int flag, int end_flag);
void play_audio(const char *filename);
void* receive_client_data(void* args);
void capture_screenshot(char *filename);
void capture_image(char *filename, int time);

extern struct sockaddr_in client_address;       // IPv4 address struct for TCP communication between ss and nfs (requests)
extern int client_sock_fd;
extern int logged_in;

#endif