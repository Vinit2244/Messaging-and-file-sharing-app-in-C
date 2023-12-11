#include "server.h"

queue pending_req_queue = NULL;     // Stores all the pending requests to be processed
pthread_mutex_t threads_arr_mutex;  // Lock to limit access to the threads array
client_info clients_list = NULL;    // Stores information about all the clients connected to the server
int n_clients = 0;                  // Number of clients server knows about

void* listening_thread(void* args)
{
    // Initialinsing queue to hold pending requests to be served
    pending_req_queue = init_queue();
    
    if (pthread_mutex_init(&threads_arr_mutex, NULL) != 0)
    {
        fprintf(stderr, REDB("pthread_mutex_init : Unable to initialise thread_arr_mutex : %s\n"), strerror(errno));
        exit(EXIT_FAILURE);
    }
    
    requests_serving_threads_arr = (pthread_t*) malloc(MAX_PENDING * sizeof(pthread_t));
    if (requests_serving_threads_arr == NULL)
    {
        fprintf(stderr, REDB("malloc : cannot allocate memory to requests thread : %s\n"), strerror(errno));
        exit(EXIT_FAILURE);
    }

    thread_slot_empty_arr = (int*) calloc(MAX_PENDING, sizeof(int));    // 0 indicates slot is empty and 1 indicates slot is busy
    if (thread_slot_empty_arr == NULL)
    {
        fprintf(stderr, REDB("malloc : cannot allocate memory to thread_slot_empty_arr : %s\n"), strerror(errno));
        exit(EXIT_FAILURE);
    }

    clients_list = (client_info) malloc(MAX_CLIENTS * sizeof(st_client_info));
    if (clients_list == NULL)
    {
        fprintf(stderr, REDB("malloc : cannot allocate memory to clients array : %s\n"), strerror(errno));
        exit(EXIT_FAILURE);
    }

    // Initialising clients list to invalid for all the clients
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        clients_list[i].valid = 0;
        clients_list[i].online = OFFLINE;
    }

    n_clients = load_all_clients();

    if (n_clients < 0)
    {
        fprintf(stderr, REDB("load_all_clients : could not load clients list : %s\n"), strerror(errno));
        exit(EXIT_FAILURE);
    }

    pthread_t backup_thread;
    if (pthread_create(&backup_thread, NULL, &backup, NULL) != 0)
    {
        fprintf(stderr, REDB("pthread_create : Unable to create backup_thread : %s\n"), strerror(errno));
        exit(EXIT_FAILURE);
    }
    
    sleep(1);

    memset(&server_address, 0, sizeof(server_address));

    // This socket id is never used for sending/receiving data, used by server just to get new sockets (clients)
    server_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket_fd < 0)
    {
        // Some error occured while creating socket
        fprintf(stderr, REDB("socket : could not start server socket : %s\n"), strerror(errno));
        exit(EXIT_FAILURE);
    }

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(SERVER_PORT_N);
    server_address.sin_addr.s_addr = INADDR_ANY;    // Any available address

    // Binding to the port
    if (bind(server_socket_fd, (struct sockaddr *)&server_address, sizeof(server_address)) == -1)
    {
        fprintf(stderr, REDB("bind : could not bind server socket : %s\n"), strerror(errno));
        exit(EXIT_FAILURE);
    }

    // Listening for incoming requests for communication
    if (listen(server_socket_fd, MAX_PENDING) == -1)
    {
        fprintf(stderr, REDB("listen : could not listen on server socket : %s\n"), strerror(errno));
        exit(EXIT_FAILURE);
    }

    printf(BHGREEN("==================== Started listening to requests ====================\n\n"));

    while (1)
    {
        // Keep accepting incoming requests
        struct sockaddr_in client_address;
        int addr_size = sizeof(struct sockaddr_in);

        int client_socket_fd = accept(server_socket_fd, (struct sockaddr *) &client_address, (socklen_t *)&addr_size);
        
        if (client_socket_fd == -1)
        {
            fprintf(stderr, REDB("accept : could not accept connection on nfs socket : %s\n"), strerror(errno));
            exit(EXIT_FAILURE);
        }

        // Searching for a free thread
        int flag = 0;   // Indicates whether a flag has been allocated or not
        pthread_mutex_lock(&threads_arr_mutex);
        for (int i = 0; i < MAX_PENDING; i++)
        {
            if (thread_slot_empty_arr[i] == 0)
            {
                thread_slot_empty_arr[i] = 1;
                thread_data args = (thread_data) malloc(sizeof(st_thread_data));
                args->client_socket_fd = client_socket_fd;
                args->thread_idx = i;

                // Create a new thread on the same position
                pthread_create(&requests_serving_threads_arr[i], NULL, &serve_request, args);
                flag = 1;
                break;
            }
        }

        if (flag == 0)
        {
            // Unable to assign a thread as all the threads are currently busy so notify the client
            st_request recvd_request;
            memset(recvd_request.data, 0, MAX_DATA_LEN);

            // Receiving the request
            int recvd_msg_size;
            recv(client_socket_fd, &recvd_request, sizeof(st_request), 0);
            insert_log(recvd_request.recv_port_no, recvd_request.request_type, recvd_request.data, REQ_RECVD, recvd_request.username, recvd_request.ip);
            send_ack(REQ_UNSERVICED, client_socket_fd, strerror(errno));
            insert_log(recvd_request.recv_port_no, recvd_request.request_type, recvd_request.data, REQ_UNSERVICED, recvd_request.username, recvd_request.ip);
        }
        pthread_mutex_unlock(&threads_arr_mutex);
    }

    if (close(server_socket_fd) < 0)
    {
        fprintf(stderr, REDB("close : failed to close the nfs socket!\n"));
        exit(EXIT_FAILURE);
    }
    destroy_queue(pending_req_queue);

    return NULL;
}

void* serve_request(void* args)
{
    st_thread_data meta_data = *((thread_data) args);
    int sock_fd = meta_data.client_socket_fd;   // Socket id for communicating with the node which has sent the request
    int thread_index = meta_data.thread_idx;    // Index of the thread on which this is running

    // Freeing arguments as all the information is extracted
    free(args);

    // Receiving and serving the request
    st_request recvd_request;
    memset(recvd_request.data, 0, MAX_DATA_LEN);

    // Receiving the request
    int recvd_msg_size;
    if ((recvd_msg_size = recv(sock_fd, &recvd_request, sizeof(st_request), 0)) <= 0)
    {
        fprintf(stderr, REDB("recv  : %s\n"), strerror(errno));
        send_ack(REQ_UNSERVICED, sock_fd, strerror(errno));
        insert_log(recvd_request.recv_port_no, recvd_request.request_type, recvd_request.data, REQ_UNSERVICED, recvd_request.username, recvd_request.ip);
        goto End;
    }
    insert_log(recvd_request.recv_port_no, recvd_request.request_type, recvd_request.data, REQ_RECVD, recvd_request.username, recvd_request.ip);

    // Serve Requests
    if (recvd_request.request_type == SIGNIN)
    {
        printf(YELLOW("Sign in request received.\n"));
        printf("Data : %s\n\n", recvd_request.data);

        // Search in the clients list for the username and match the password and send response accordingly
        // Searching for username
        int flag = 0;
        for (int i = 0; i < n_clients; i++)
        {
            if (clients_list[i].valid == 0)
            {
                // Invalid client
                continue;
            }

            if (strcmp(clients_list[i].client_username, recvd_request.username) == 0)
            {
                // Username found
                flag = 1;
                // Checking for password
                if (strcmp(clients_list[i].client_password, recvd_request.data) == 0)
                {
                    // Password matched
                    send_ack(ACK, sock_fd, BHGREEN("Successfully signed in\n"));
                    insert_log(recvd_request.recv_port_no, recvd_request.request_type, recvd_request.data, ACK, recvd_request.username, recvd_request.ip);
                    clients_list[i].online = ONLINE;
                    clients_list[i].client_recv_port_n = recvd_request.recv_port_no;
                    strcpy(clients_list[i].client_ip, recvd_request.ip);
                    clients_list[i].client_recv_port_n = recvd_request.recv_port_no;
                    goto End;
                }
                else
                {
                    // Password did not match
                    send_ack(SIGNIN_FAILED, sock_fd, "Incorrect password\n");
                    insert_log(recvd_request.recv_port_no, recvd_request.request_type, recvd_request.data, SIGNIN_FAILED, recvd_request.username, recvd_request.ip);
                    goto End;
                }
            }
        }
        // Username not found
        if (flag == 0)
        {
            // Username does not exist
            send_ack(SIGNIN_FAILED, sock_fd, REDB("Username does not exist\n"));
            insert_log(recvd_request.recv_port_no, recvd_request.request_type, recvd_request.data, SIGNIN_FAILED, recvd_request.username, recvd_request.ip);
            goto End;
        }
    }
    else if (recvd_request.request_type == SIGNUP)
    {
        printf(YELLOW("Sign up request received.\n"));
        printf("Data : %s\n\n", recvd_request.data);

        // Search if the username already exists in the clients list and send response accordingly
        // Searching for username
        int flag = 0;
        for (int i = 0; i < n_clients; i++)
        {
            if (clients_list[i].valid == 0)
            {
                // Invalid client
                continue;
            }

            if (strcmp(clients_list[i].client_username, recvd_request.username) == 0)
            {
                // Username already exists
                flag = 1;
                break;
            }
        }
        // Username found
        if (flag == 1)
        {
            // Username already exists
            send_ack(SIGNUP_FAILED, sock_fd, REDB("Username already exists\n"));
            insert_log(recvd_request.recv_port_no, recvd_request.request_type, recvd_request.data, SIGNUP_FAILED, recvd_request.username, recvd_request.ip);
            goto End;
        }
        // Username not found
        if (flag == 0)
        {
            // Username does not exist
            // Adding the new client to the clients list
            for (int i = 0; i < MAX_CLIENTS; i++)
            {
                if (clients_list[i].valid == 0)
                {
                    // Found an empty slot
                    strcpy(clients_list[i].client_username, recvd_request.username);
                    strcpy(clients_list[i].client_password, recvd_request.data);
                    strcpy(clients_list[i].client_ip, recvd_request.ip);
                    clients_list[i].client_recv_port_n = recvd_request.recv_port_no;
                    clients_list[i].valid = 1;
                    clients_list[i].online = ONLINE;
                    n_clients++;

                    // Sending the response
                    send_ack(ACK, sock_fd, BHGREEN("Successfully signed up\n"));
                    insert_log(recvd_request.recv_port_no, recvd_request.request_type, recvd_request.data, ACK, recvd_request.username, recvd_request.ip);
                    n_clients++;
                    goto End;
                }
            }

            send_ack(SIGNUP_FAILED, sock_fd, REDB("Server is full\n"));
            insert_log(recvd_request.recv_port_no, recvd_request.request_type, recvd_request.data, SIGNUP_FAILED, recvd_request.username, recvd_request.ip);
        }
    }
    else if (recvd_request.request_type == SIGNOUT)
    {
        printf(YELLOW("Sign out request received.\n"));
        printf("Data : %s\n\n", recvd_request.data);

        for (int i = 0; i < n_clients; i++)
        {
            if (clients_list[i].valid == 0)
            {
                // Invalid client
                continue;
            }

            if (strcmp(clients_list[i].client_username, recvd_request.username) == 0)
            {
                // Username found
                clients_list[i].online = OFFLINE;
                send_ack(ACK, sock_fd, BHGREEN("Successfully signed out\n"));
                insert_log(recvd_request.recv_port_no, recvd_request.request_type, recvd_request.data, ACK, recvd_request.username, recvd_request.ip);
                goto End;
            }
        }

        // Username not found
        send_ack(SIGNOUT_FAILED, sock_fd, REDB("Username does not exist\n"));
    }
    else if (recvd_request.request_type == DELETE_ACCOUNT)
    {
        printf(YELLOW("Delete account request received.\n"));
        printf("Data : %s\n\n", recvd_request.data);

        for (int i = 0; i < n_clients; i++)
        {
            if (clients_list[i].valid == 0)
            {
                // Invalid client
                continue;
            }

            if (strcmp(clients_list[i].client_username, recvd_request.username) == 0)
            {
                // Username found
                // Check for password
                if (strcmp(clients_list[i].client_password, recvd_request.data) != 0)
                {
                    // Password did not match
                    send_ack(DELETE_FAILED, sock_fd, REDB("Incorrect password\n"));
                    insert_log(recvd_request.recv_port_no, recvd_request.request_type, recvd_request.data, DELETE_FAILED, recvd_request.username, recvd_request.ip);
                    goto End;
                }
                clients_list[i].valid = 0;
                send_ack(ACK, sock_fd, BHGREEN("Successfully deleted account\n"));
                insert_log(recvd_request.recv_port_no, recvd_request.request_type, recvd_request.data, ACK, recvd_request.username, recvd_request.ip);
                n_clients--;
                goto End;
            }
        }

        // Username not found
        send_ack(DELETE_FAILED, sock_fd, REDB("Username does not exist\n"));
        insert_log(recvd_request.recv_port_no, recvd_request.request_type, recvd_request.data, DELETE_FAILED, recvd_request.username, recvd_request.ip);
    }
    else if (recvd_request.request_type == SEND_IMG)
    {
        printf(YELLOW("Send image request received.\n"));
        printf("Data : %s\n\n", recvd_request.data);
        
        for (int i = 0; i < MAX_CLIENTS; i++)
        {
            if (clients_list[i].valid == 0)
            {
                // Invalid client
                continue;
            }
            if (strcmp(clients_list[i].client_username, recvd_request.send_to_username) == 0)
            {
                // Username found
                printf(GREEN("Username found\n"));
                if (clients_list[i].online == ONLINE)
                {
                    // Client is online
                    printf(GREEN("Client is online\n"));

                    send_ack(ACK, sock_fd, NULL);
                    printf(GREEN("Sent online ack\n"));
                    insert_log(recvd_request.recv_port_no, recvd_request.request_type, recvd_request.data, ACK, recvd_request.username, recvd_request.ip);

                    int client2_sock_fd = connect_to_client(clients_list[i].client_ip, clients_list[i].client_recv_port_n);
                    if (client2_sock_fd < 0)
                    {
                        fprintf(stderr, REDB("connnect_to_client : could not connect to client : %s\n"), strerror(errno));
                        exit(EXIT_FAILURE);
                    }
                    printf(GREEN("Connected to client2\n"));

                    while(1)
                    {
                        st_request intermediate_st;
                        memset(intermediate_st.data, 0, MAX_DATA_LEN);
                        memset(intermediate_st.send_to_username, 0, 1024);
                        memset(intermediate_st.send_from_username, 0, 1024);

                        int recvd_msg_size;
                        if ((recvd_msg_size = recv(sock_fd, &intermediate_st, sizeof(st_request), 0)) <= 0)
                        {
                            fprintf(stderr, REDB("recv  : %s\n"), strerror(errno));
                            send_ack(REQ_UNSERVICED, sock_fd, strerror(errno));
                            insert_log(recvd_request.recv_port_no, recvd_request.request_type, recvd_request.data, REQ_UNSERVICED, recvd_request.username, recvd_request.ip);
                            goto End;
                        }
                        insert_log(recvd_request.recv_port_no, recvd_request.request_type, recvd_request.data, INTER_REQ_RECV, recvd_request.username, recvd_request.ip);

                        int sent_msg_size;
                        if ((sent_msg_size = send(client2_sock_fd, &intermediate_st, sizeof(st_request), 0)) <= 0)
                        {
                            fprintf(stderr, REDB("send : could not send data to client : %s\n"), strerror(errno));
                            send_ack(REQ_UNSERVICED, sock_fd, strerror(errno));
                            insert_log(recvd_request.recv_port_no, recvd_request.request_type, recvd_request.data, REQ_UNSERVICED, recvd_request.username, recvd_request.ip);
                            goto End;
                        }
                        insert_log(recvd_request.recv_port_no, recvd_request.request_type, recvd_request.data, INTER_REQ_SEND, recvd_request.username, recvd_request.ip);

                        printf(GREEN("Sent intermediate request\n"));

                        if (intermediate_st.request_type == IMG_DATA_END)
                        {
                            printf(GREEN("Received image data end\n"));
                            break;
                        }

                        if (intermediate_st.request_type == FAIL)
                        {
                            printf(RED("Client failed to receive image\n"));
                            send_ack(FAIL, client2_sock_fd, NULL);
                            break;
                        }                
                    }

                    close(client2_sock_fd);
                }
                else
                {
                    st_request req;
                    req.request_type = CLIENT_OFFLINE;
                    send_ack(CLIENT_OFFLINE, sock_fd, RED("Client is offline\n"));
                    insert_log(recvd_request.recv_port_no, recvd_request.request_type, recvd_request.data, CLIENT_OFFLINE, recvd_request.username, recvd_request.ip);
                    goto End;
                }
            }
        }

        // Username not found
        send_ack(INVALID_USER, sock_fd, REDB("Username does not exist\n"));
        insert_log(recvd_request.recv_port_no, recvd_request.request_type, recvd_request.data, INVALID_USER, recvd_request.username, recvd_request.ip);
    }
    else
    {
        printf(REDB("Invalid request type received.\n"));
        printf("Data : %s\n\n", recvd_request.data);

        // Invalid request
        send_ack(REQ_UNSERVICED, sock_fd, REDB("Invalid request type\n"));
        insert_log(recvd_request.recv_port_no, recvd_request.request_type, recvd_request.data, REQ_UNSERVICED, recvd_request.username, recvd_request.ip);
    }

End:
    // Closing client socket as all the communication is done
    if (close(sock_fd) < 0)
    {
        fprintf(stderr, REDB("close : failed to close the client socket!\n"));
        exit(EXIT_FAILURE);
    }

    // Freeing thread slot
    pthread_mutex_lock(&threads_arr_mutex);
    thread_slot_empty_arr[thread_index] = 0;
    pthread_mutex_unlock(&threads_arr_mutex);

    return NULL;
}

void* backup(void* args)
{
    while (1)
    {
        // Backup the clients list
        FILE* fp = fopen("clients_list.txt", "w");
        if (fp == NULL)
        {
            fprintf(stderr, REDB("fopen : could not open clients_list.txt : %s\n"), strerror(errno));
            exit(EXIT_FAILURE);
        }

        for (int i = 0; i < MAX_CLIENTS; i++)
        {
            if (clients_list[i].valid == 0)
            {
                // Invalid client
                continue;
            }

            fprintf(fp, "%s|%s|%d|%s\n", clients_list[i].client_username, clients_list[i].client_password, clients_list[i].client_recv_port_n, clients_list[i].client_ip);
        }

        fclose(fp);

        sleep(2);
    }
}

int load_all_clients()
{
    FILE* fp = fopen("clients_list.txt", "r");
    if (fp == NULL)
    {
        return 0;
    }

    char* line = NULL;
    size_t len = 0;
    ssize_t read;

    int i = 0;
    while ((read = getline(&line, &len, fp)) != -1)
    {
        char **tokens = tokenize(line, '|');
        strcpy(clients_list[i].client_username, tokens[0]);
        strcpy(clients_list[i].client_password, tokens[1]);
        clients_list[i].client_recv_port_n = atoi(tokens[2]);
        strcpy(clients_list[i].client_ip, tokens[3]);
        clients_list[i].valid = 1;
        clients_list[i].online = OFFLINE;
        i++;
    }

    fclose(fp);

    return i;
}
