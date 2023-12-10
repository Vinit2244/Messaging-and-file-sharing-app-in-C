#ifndef _SERVER_H_
#define _SERVER_H_

#include "../common_headers/headers.h"
#include "../utils/logging.h"

// ===================== Client Info Structure =====================
typedef struct st_client_info
{
    int valid;
    int online;
    char client_username[1024];
    char client_password[1024];
    char client_ip[1024];
    int client_recv_port_n;
} st_client_info;

typedef struct st_client_info* client_info;

// ===================== Thread Structure =====================
typedef struct st_thread_data
{
    int client_socket_fd;
    int thread_idx;
} st_thread_data;

typedef struct st_thread_data* thread_data;

// ===================== Threads Definitions =====================
void* listening_thread(void* args);
void* serve_request(void* args);
void* backup(void* args);

int load_all_clients();

extern char* PWD;                               // Current working directory
extern pthread_t* requests_serving_threads_arr; // Holds the threads
extern int* thread_slot_empty_arr;              // 1 = thread working, 0 = thread available
extern struct sockaddr_in server_address;       // IPv4 address struct for TCP communication between ss and nfs (requests)
extern int server_socket_fd;
extern int n_clients;

extern client_info clients_list;
extern char clear[10];

#endif