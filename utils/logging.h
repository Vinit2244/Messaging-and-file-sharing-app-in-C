#ifndef _LOGGING_H_
#define _LOGGING_H_

#include "../common_headers/headers.h"

void handleCtrlZ(int signum);
int insert_log(const int ss_or_client_port, const int request_type, const char* request_data, const int status_code, const char* username, const char* ip);

#endif