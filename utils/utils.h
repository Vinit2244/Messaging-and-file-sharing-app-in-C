#ifndef _UTILS_H_
#define _UTILS_H_

#include "../common_headers/headers.h"

void send_ack(const int status_code, const int sock_fd, const char* msg);
char **tokenize(const char *str, const char ch);
void free_tokens(char **tokens);
void press_enter_to_contiue();
void detect_os(char* clear);
int connect_to_server();
int connect_to_client(const char* ip, const int port);

#endif