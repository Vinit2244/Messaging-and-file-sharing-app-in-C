#ifndef _HEADERS_H_
#define _HEADERS_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <curl/curl.h>
#include <signal.h>
#include <pthread.h>

#include "../utils/queue.h"
#include "color_codes.h"
#include "../utils/utils.h"
#include "config.h"
#include "req_codes.h"

// ===================== Request Structure =====================
typedef struct st_request
{
    int request_type;
    char data[MAX_DATA_LEN];
    int recv_port_no;
    char ip[20];
    char username[1024];
    char send_to_username[1024];
    char send_from_username[1024];
} st_request;

typedef struct st_request* request;

#endif