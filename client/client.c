#include "client.h"

struct sockaddr_in client_address;       // IPv4 address struct for TCP communication between ss and nfs (requests)
int client_sock_fd;

int client_receiving_port_n = 0;
int credentials_saved = 0;
char* my_ip = NULL;
char clear[10] = {0};

char username[1024] = {0};
char password[1024] = {0};

int main(int argc, char* argv[])
{
    // Handling Ctrl + c (SIGINT) signal to logout and exit the server
    struct sigaction sa;
    sa.sa_handler = &handleCtrlC;  // Ctrl + S (Windows/Linux/Mac)
    sa.sa_flags = SA_RESTART;      // Automatically restart the system call
    sigaction(SIGINT, &sa, NULL);  // Ctrl + C sends SIGINT signal - Prints the log onto the screen

    detect_os(clear);

    system(clear);

    // Asking for the port number which the client wants to run on
    printf(YELLOW("Enter receiving port number for client: "));
    scanf("%d", &client_receiving_port_n);

    pthread_t client_receiving_thread;
    pthread_create(&client_receiving_thread, NULL, receive_client_data, NULL);

    int choice;

    // Getting the ip info
    my_ip = get_my_ip();
    if (my_ip == NULL)
    {
        fprintf(stderr, REDHB("Could not get the ip address of the client\n"));
        exit(EXIT_FAILURE);
    }

    printf(GREEN("Your ip is : %s\n"), my_ip);
    press_enter_to_contiue();

    // First try to read the credentials from the file, if not available then ask for the credentials
    FILE* fp = fopen("credentials.txt", "r");
    if (fp == NULL)
    {
ASK_SIGNIN_OR_SIGNUP:
        // Clear the terminal
        system(clear);

        // No credentials file found
        // Asking user if he wants to signin or signup
        printf(PINK("What do you want to do?\n1. Signin\n2. Signup\n"));
        printf(YELLOW("\nEnter your choice: "));
        scanf("%d", &choice);

        if (choice == 1)
        {
            // Signin
            memset(username, 0, sizeof(username));
            memset(password, 0, sizeof(password));

            // Asking for username and password
            printf(BLUE("\nEnter the username(email): "));
            scanf("%s", username);
            printf(BLUE("Enter the password: "));
            scanf("%s", password);

            // Sends sign in request to the server and then checks if the signin suceeded or not
            int result = send_signin_req(); // Send the signin request to the server and then check if the signin suceeded or not
            press_enter_to_contiue();

            if (result == 0)
            {
                // sign in is successful
                if (credentials_saved == 0)
                    ask_to_save_credentials();
                goto SIGNIN_SUCCESSFUL;
            }
            else
            {
                // sign in failed
                goto ASK_SIGNIN_OR_SIGNUP;
            }
        }
        else if (choice == 2)
        {
            // Signup
            memset(username, 0, sizeof(username));
            memset(password, 0, sizeof(password));
            printf(BLUE("\nEnter the username(email): "));
            scanf("%s", username);
            printf(BLUE("Enter the password: "));
            scanf("%s", password);

            int result = send_signup_req(); // Register the new username and password and then check if the registration suceeded or not
            press_enter_to_contiue();

            if (result == 0)
            {
                // sign up is successful
                ask_to_save_credentials();
                goto SIGNIN_SUCCESSFUL;
            }
            else
            {
                // sign up failed
                goto ASK_SIGNIN_OR_SIGNUP;
            }
        }
        else
        {
            char temp[100];
            printf(REDHB("Invalid choice\n"));
            press_enter_to_contiue();
            goto ASK_SIGNIN_OR_SIGNUP;
        }
    }
    else
    {
        credentials_saved = 1;
        // Credentials file found
        // Read the credentials from the file
        memset(username, 0, sizeof(username));
        memset(password, 0, sizeof(password));
        fscanf(fp, "%s\n%s\n", username, password);
        fclose(fp);

        int result = send_signin_req();
        if (result != 0)
        {
            // If the signin fails it means the credentials stored are not right so just remove that file
            remove("credentials.txt");
            goto ASK_SIGNIN_OR_SIGNUP;
        }
        else
        {
            // Signin successful
            goto SIGNIN_SUCCESSFUL;
        }
    }
SIGNIN_SUCCESSFUL:
    // Signin successful

    system(clear);
    printf(ORANGE("What do you want to do?\n"));
    printf(CYAN("1. Send Message\n"));
    printf(CYAN("2. Send File\n"));
    printf(CYAN("3. Send Image\n"));
    printf(CYAN("4. Signout\n"));
    printf(CYAN("5. Delete Account\n"));
    printf(CYAN("6. Exit\n"));

    int choice2;
    printf(YELLOW("\nEnter your choice: "));
    scanf("%d", &choice2);

    printf("\n");

    if (choice2 == 1)
    {
        // Send message
        printf("Enter the username of the person you want to send the message to: ");
        char to_username[1024] = {0};
        scanf("%s", to_username);

        printf("Enter the message: ");
        char message[1024] = {0};
        scanf("%s", message);

    }
    else if (choice2 == 2)
    {
        // Send file
    }
    else if (choice2 == 3)
    {
        // Send image
        printf("Enter the username of the person you want to send the image to: ");
        char to_username[1024] = {0};
        scanf("%s", to_username);

        st_request image_req_st;
        image_req_st.request_type = SEND_IMG;
        memset(image_req_st.send_from_username, 0, sizeof(image_req_st.send_from_username));
        memset(image_req_st.send_to_username, 0, sizeof(image_req_st.send_to_username));

        strcpy(image_req_st.send_from_username, username);
        strcpy(image_req_st.send_to_username, to_username);

        memset(image_req_st.data, 0, MAX_DATA_LEN);

        int socket_fd = connect_to_server();

        // Sending the Send Image request
        int sent_msg_size;
        if ((sent_msg_size = send(socket_fd, (request) &image_req_st, sizeof(st_request), 0)) < 0)
        {
            fprintf(stderr, REDHB("send : could not send client Send Image request : %s\n"), strerror(errno));
            press_enter_to_contiue();
            goto SIGNIN_SUCCESSFUL;
        }

        // Receiving the response
        int recvd_msg_size;
        st_request image_response_st;
        if ((recvd_msg_size = recv(socket_fd, (request) &image_response_st, sizeof(st_request), 0)) < 0)
        {
            fprintf(stderr, REDHB("recv : could not recv client Send Image request : %s\n"), strerror(errno));
            press_enter_to_contiue();
            goto SIGNIN_SUCCESSFUL;
        }

        if (image_response_st.request_type == ACK)
        {
            printf(GREEN("User to be sent image to found!\n"));

            // Send Image request successful
            // Image can be sent
            printf("Enter the path of the image: ");
            char image_path[1024] = {0};
            scanf("%s", image_path);

            printf(GREEN("Sending image...\n"));

            FILE* image = fopen(image_path, "rb");
            if (image == NULL)
            {
                fprintf(stderr, REDHB("fopen : could not open the image file : %s\n"), strerror(errno));
                st_request failed_st;
                failed_st.request_type = FAIL;
                send(socket_fd, (st_request*) &failed_st, sizeof(st_request), 0);
                close(socket_fd);
                press_enter_to_contiue();
                goto SIGNIN_SUCCESSFUL;
            }

            // Send the image data
            st_request image_data;
            image_data.request_type = IMG_DATA;
            memset(image_data.data, 0, MAX_DATA_LEN);
            memset(image_data.send_from_username, 0, sizeof(image_data.send_from_username));
            memset(image_data.send_to_username, 0, sizeof(image_data.send_to_username));

            strcpy(image_data.send_from_username, username);
            strcpy(image_data.send_to_username, to_username);

            int bytesRead = fread(image_data.data, 1, MAX_DATA_LEN, image);
            if (bytesRead <= 0)
            {
                fprintf(stderr, REDHB("fread : could not read the image file : %s\n"), strerror(errno));
                st_request failed_st;
                failed_st.request_type = FAIL;
                send(socket_fd, (st_request*) &failed_st, sizeof(st_request), 0);
                close(socket_fd);
                press_enter_to_contiue();
                goto SIGNIN_SUCCESSFUL;
            }

            send(socket_fd, (request) &image_data, sizeof(st_request), 0);
            printf("Sent image packet\n");

            while(1)
            {
                memset(image_data.data, 0, MAX_DATA_LEN);
                int bytes_read = fread(image_data.data, 1, MAX_DATA_LEN, image);
                if (bytes_read <= 0)
                {
                    break;
                }
                send(socket_fd, (request) &image_data, sizeof(st_request), 0);
                printf("Sent image packet\n");
            }

            // Send the end of image data
            st_request image_data_end;
            image_data_end.request_type = IMG_DATA_END;

            send(socket_fd, (request) &image_data_end, sizeof(st_request), 0);

            fclose(image);

            printf(GREEN("Image sent successfully!\n"));
        }
        else
        {
            // Send Image request failed
            printf(REDHB("Send Image request failed!\n"));
            printf(REDHB("Reason: %s"), image_response_st.data);
            close(socket_fd);
            press_enter_to_contiue();
            goto SIGNIN_SUCCESSFUL;
        }
    }
    else if (choice2 == 4)
    {
        // signout
        printf(ORANGE("Signing out...\n"));
        int result = send_signout_req();
        press_enter_to_contiue();
        if (result == 0)
        {
            goto ASK_SIGNIN_OR_SIGNUP;
        }
        else
        {
            goto SIGNIN_SUCCESSFUL;
        }
    }
    else if (choice2 == 5)
    {
        // Delete account
        printf(REDHB("Are you sure you want to delete your account? (y/n): "));
        char temp[2] = {0};
        scanf("%s", temp);
        if (strcmp(temp, "y") == 0)
        {
            printf("Enter your password to confirm: ");
            char entered_password[1024];
            scanf("%s", entered_password);
            if (strcmp(entered_password, password) != 0)
            {
                printf(REDHB("Incorrect password\n"));
                press_enter_to_contiue();
                goto SIGNIN_SUCCESSFUL;
            }
            int result = send_delete_account_req();
            if (result == 0)
            {
                // Account deleted succesfully
                remove("credentials.txt");
                credentials_saved = 0;
                press_enter_to_contiue();
                goto ASK_SIGNIN_OR_SIGNUP;
            }
            else
            {
                // Account deletion failed
                press_enter_to_contiue();
                goto SIGNIN_SUCCESSFUL;
            }
        }
        else
            goto SIGNIN_SUCCESSFUL;
    }
    else if (choice2 == 6)
    {
        // exit
        printf(ORANGE("Exiting...\n"));
        int result;
        while ((result = send_signout_req()) != 0)
        {
            ;   // Continue
        }
        printf(GREEN("Exitted!\n"));
        press_enter_to_contiue();
        exit(EXIT_SUCCESS);
    }
    else
    {
        printf(REDHB("Invalid choice\n"));
        press_enter_to_contiue();
        goto SIGNIN_SUCCESSFUL;
    }

    return 0;
}

// Function to send the sign up request to the server, returns 0 if successful else 1
int send_signup_req()
{
    // Preparing the request to be sent
    st_request signup_req_st;
    signup_req_st.request_type = SIGNUP;
    signup_req_st.recv_port_no = client_receiving_port_n;
    memset(signup_req_st.ip, 0, sizeof(signup_req_st.ip));
    memset(signup_req_st.username, 0, sizeof(signup_req_st.username));
    strcpy(signup_req_st.ip, my_ip);
    strcpy(signup_req_st.username, username);
    memset(signup_req_st.data, 0, MAX_DATA_LEN);

    sprintf(signup_req_st.data, "%s", password); // <My password>

    int socket_fd = connect_to_server();

    // Sending the Sign Up request
    int sent_msg_size;
    if ((sent_msg_size = send(socket_fd, (request) &signup_req_st, sizeof(st_request), 0)) < 0)
    {
        fprintf(stderr, REDHB("send : could not send client Sign Up request : %s\n"), strerror(errno));
        return 1;
    }

    int recvd_msg_size;
    st_request signup_response_st;
    if ((recvd_msg_size = recv(socket_fd, (request) &signup_response_st, sizeof(st_request), 0)) < 0)
    {
        fprintf(stderr, REDHB("recv : could not recv client Sign Up request : %s\n"), strerror(errno));
        return 1;
    }

    if (signup_response_st.request_type == ACK)
    {
        // Sign Up successful
        printf(GREEN("Sign Up successful!\n"));
    }
    else
    {
        // Sign Up failed
        printf(REDHB("Sign Up failed!\n"));
        printf(REDHB("Reason: %s"), signup_response_st.data);
        close(socket_fd);
        return 1;
    }

    // Closing the socket as the communication is done
    close(socket_fd);

    return 0;
}

// Function to send the signout request to the server
int send_signout_req()
{
    st_request signout_req_st;
    signout_req_st.request_type = SIGNOUT;
    signout_req_st.recv_port_no = client_receiving_port_n;
    memset(signout_req_st.ip, 0, sizeof(signout_req_st.ip));
    memset(signout_req_st.username, 0, sizeof(signout_req_st.username));
    strcpy(signout_req_st.ip, my_ip);
    strcpy(signout_req_st.username, username);

    int socket_fd = connect_to_server();

    // Sending the Sign Out request
    int sent_msg_size;
    if ((sent_msg_size = send(socket_fd, (request) &signout_req_st, sizeof(st_request), 0)) < 0)
    {
        fprintf(stderr, REDHB("send : could not send client Sign Out request : %s\n"), strerror(errno));
        return 1;
    }

    int recvd_msg_size;
    st_request signout_response_st;
    if ((recvd_msg_size = recv(socket_fd, (request) &signout_response_st, sizeof(st_request), 0)) < 0)
    {
        fprintf(stderr, REDHB("recv : could not recv client Sign Out request : %s\n"), strerror(errno));
        return 1;
    }

    if (signout_response_st.request_type == ACK)
    {
        // Sign Out successful
        printf(GREEN("Sign Out successful!\n"));
    }
    else
    {
        // Sign Out failed
        printf(REDHB("Sign Out failed!\n"));
        printf(REDHB("Reason: %s"), signout_response_st.data);
        close(socket_fd);
        return 1;
    }

    // Closing the socket as the communication is done
    close(socket_fd);

    return 0;
}

// Function to send the sign in request to the server, returns 0 if successful else 1
int send_signin_req()
{
    // Preparing the request to be sent
    st_request signin_req_st;
    signin_req_st.request_type = SIGNIN;
    signin_req_st.recv_port_no = client_receiving_port_n;
    memset(signin_req_st.ip, 0, sizeof(signin_req_st.ip));
    memset(signin_req_st.username, 0, sizeof(signin_req_st.username));
    strcpy(signin_req_st.ip, my_ip);
    strcpy(signin_req_st.username, username);
    memset(signin_req_st.data, 0, MAX_DATA_LEN);

    sprintf(signin_req_st.data, "%s", password); // <My password>
 
    int socket_fd = connect_to_server();

    // Sending the Sign In request
    int sent_msg_size;
    if ((sent_msg_size = send(socket_fd, (request) &signin_req_st, sizeof(st_request), 0)) < 0)
    {
        fprintf(stderr, REDHB("send : could not send client Sign In request : %s\n"), strerror(errno));
        return 1;
    }

    int recvd_msg_size;
    st_request signin_response_st;
    if ((recvd_msg_size = recv(socket_fd, (request) &signin_response_st, sizeof(st_request), 0)) < 0)
    {
        fprintf(stderr, REDHB("recv : could not recv client Sign In request : %s\n"), strerror(errno));
        return 1;
    }

    if (signin_response_st.request_type == ACK)
    {
        // Sign In successful
        printf(GREEN("Sign In successful!\n"));
    }
    else
    {
        // Sign In failed
        printf(REDHB("Sign In failed!\n"));
        printf(REDHB("Reason: %s"), signin_response_st.data);
        close(socket_fd);
        return 1;
    }

    // Closing the socket as the communication is done
    close(socket_fd);

    return 0;
}

// Function to send the delete account request to the server, returns 0 if successful else 1
int send_delete_account_req()
{
    st_request delete_account_req_st;
    delete_account_req_st.request_type = DELETE_ACCOUNT;
    delete_account_req_st.recv_port_no = client_receiving_port_n;
    memset(delete_account_req_st.ip, 0, sizeof(delete_account_req_st.ip));
    memset(delete_account_req_st.username, 0, sizeof(delete_account_req_st.username));
    strcpy(delete_account_req_st.ip, my_ip);
    strcpy(delete_account_req_st.username, username);

    memset(delete_account_req_st.data, 0, MAX_DATA_LEN);
    sprintf(delete_account_req_st.data, "%s", password);

    int socket_fd = connect_to_server();

    // Sending the Delete Account request
    int sent_msg_size;
    if ((sent_msg_size = send(socket_fd, (request) &delete_account_req_st, sizeof(st_request), 0)) < 0)
    {
        fprintf(stderr, REDHB("send : could not send client Delete Account request : %s\n"), strerror(errno));
        return 1;
    }

    int recvd_msg_size;
    st_request delete_account_response_st;
    if ((recvd_msg_size = recv(socket_fd, (request) &delete_account_response_st, sizeof(st_request), 0)) < 0)
    {
        fprintf(stderr, REDHB("recv : could not recv client Delete Account request : %s\n"), strerror(errno));
        return 1;
    }

    if (delete_account_response_st.request_type == ACK)
    {
        // Delete Account successful
        printf(GREEN("Delete Account successful!\n"));
    }
    else
    {
        // Delete Account failed
        printf(REDHB("Delete Account failed!\n"));
        printf(REDHB("Reason: %s"), delete_account_response_st.data);
        close(socket_fd);
        return 1;
    }

    // Closing the socket as the communication is done
    close(socket_fd);

    return 0;
}

// Callback function to handle the response data
size_t write_callback(void *contents, size_t size, size_t nmemb, char **result)
{
    size_t total_size = size * nmemb;
    *result = realloc(*result, total_size + 1);

    if (*result == NULL)
    {
        fprintf(stderr, "Failed to allocate memory\n");
        return 0;
    }

    memcpy(*result, contents, total_size);
    (*result)[total_size] = '\0';

    return total_size;
}

// Function to get the ip address of the client
char* get_my_ip()
{
    // Getting the ip address of the client
    CURL *curl;
    CURLcode res;
    char *ip_address = NULL;

    // Initialize the curl library
    curl_global_init(CURL_GLOBAL_DEFAULT);

    // Create a curl handle
    curl = curl_easy_init();

    if (curl) {
        // Set the URL to ipinfo.io
        curl_easy_setopt(curl, CURLOPT_URL, "http://ipinfo.io/ip");

        // Set the callback function to handle the response
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &ip_address);

        // Perform the HTTP request
        res = curl_easy_perform(curl);

        // Check for errors
        if (res != CURLE_OK) {
            fprintf(stderr, REDHB("curl_easy_perform() failed: %s\n"), curl_easy_strerror(res));
            return NULL;
        } else {
            // Got the ip address
        }

        // Clean up
        curl_easy_cleanup(curl);
    } else {
        fprintf(stderr, REDHB("Failed to initialize curl\n"));
        return NULL;
    }

    // Clean up the global curl environment
    curl_global_cleanup();

    // Free the allocated memory
    return ip_address;
}

// Function to ask the user if he wants to save the credentials or not and then save the credentials if he wants to
void ask_to_save_credentials()
{
    system(clear);

    // Ask the user if we should remember him or not?
    printf(PINK("Do you want to remember your credentials? (y/n): "));
    char temp[100];
    scanf("%s", temp);
    if (strcmp(temp, "y") == 0)
    {
        // Remember the credentials (write in a file)
        FILE* fp = fopen("credentials.txt", "w");
        if (fp == NULL)
        {
            fprintf(stderr, REDHB("fopen : could not open the credentials file : %s\n"), strerror(errno));
            exit(EXIT_FAILURE);
        }
        else
        {
            fprintf(fp, "%s\n%s\n", username, password);
            fclose(fp);
        }
        credentials_saved = 1;
    }
    else
    {
        // Do not remember the credentials
    }

    return;
}

// Signout and close the program
void handleCtrlC(int signum)
{
    st_request signout_req_st;
    signout_req_st.request_type = SIGNOUT;
    signout_req_st.recv_port_no = client_receiving_port_n;
    memset(signout_req_st.ip, 0, sizeof(signout_req_st.ip));
    memset(signout_req_st.username, 0, sizeof(signout_req_st.username));
    strcpy(signout_req_st.ip, my_ip);
    strcpy(signout_req_st.username, username);

    int socket_fd = connect_to_server();

    int sent_msg_size;
    if ((sent_msg_size = send(socket_fd, (request) &signout_req_st, sizeof(st_request), 0)) < 0)
    {
        fprintf(stderr, REDHB("send : could not send client Sign Out request : %s\n"), strerror(errno));
    }

    int recvd_msg_size;
    st_request signout_response_st;
    if ((recvd_msg_size = recv(socket_fd, (request) &signout_response_st, sizeof(st_request), 0)) < 0)
    {
        fprintf(stderr, REDHB("recv : could not recv client Sign Out request ack : %s\n"), strerror(errno));
    }

    // Closing the socket as the communication is done
    close(socket_fd);

    exit(EXIT_SUCCESS);
}

// Function to receive the data from the server
void* receive_client_data(void* args)
{
    memset(&client_address, 0, sizeof(client_address));

    // This socket id is never used for sending/receiving data, used by server just to get new sockets (clients)
    client_sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_sock_fd < 0)
    {
        // Some error occured while creating socket
        fprintf(stderr, REDB("socket : could not start server socket : %s\n"), strerror(errno));
        exit(EXIT_FAILURE);
    }

    client_address.sin_family = AF_INET;
    client_address.sin_port = htons(client_receiving_port_n);
    client_address.sin_addr.s_addr = INADDR_ANY;    // Any available address

    // Binding to the port
    if (bind(client_sock_fd, (struct sockaddr *) &client_address, sizeof(client_address)) == -1)
    {
        fprintf(stderr, REDB("bind : could not bind server socket : %s\n"), strerror(errno));
        exit(EXIT_FAILURE);
    }

    // Listening for incoming requests for communication
    if (listen(client_sock_fd, MAX_PENDING) == -1)
    {
        fprintf(stderr, REDB("listen : could not listen on server socket : %s\n"), strerror(errno));
        exit(EXIT_FAILURE);
    }

    printf(BHGREEN("==================== Started listening to incoming data ====================\n\n"));

    while (1)
    {
        // Keep accepting incoming requests
        struct sockaddr_in addr;
        int addr_size = sizeof(struct sockaddr_in);

        int sock_fd = accept(client_sock_fd, (struct sockaddr *) &addr, (socklen_t *)&addr_size);
        
        if (sock_fd == -1)
        {
            fprintf(stderr, REDB("accept : could not accept connection on nfs socket : %s\n"), strerror(errno));
            exit(EXIT_FAILURE);
        }

        // Receiving the data
        st_request recvd_data;
        memset(recvd_data.data, 0, MAX_DATA_LEN);
        memset(recvd_data.send_from_username, 0, 1024);
        memset(recvd_data.send_to_username, 0, 1024);

        int recvd_msg_size;
        if ((recvd_msg_size = recv(sock_fd, &recvd_data, sizeof(st_request), 0)) <= 0)
        {
            fprintf(stderr, REDB("recv  : %s\n"), strerror(errno));
            exit(EXIT_FAILURE);
        }

        // If the data sent is image data
        if (recvd_data.request_type == IMG_DATA)
        {
            // Clear the terminal and print that we are receiving the image
            system(clear);
            printf("Receiving image from %s ...\n", recvd_data.send_from_username);

            FILE *received_image = fopen("received_image.jpg", "wb");
            if (!received_image)
            {
                fprintf(stderr, REDHB("Error opening file to save received image"));
                exit(EXIT_FAILURE);
            }

            fwrite(recvd_data.data, 1, MAX_DATA_LEN, received_image);

            // Keep receving the image data until we get the end of image data
            while(1)
            {
                memset(recvd_data.data, 0, MAX_DATA_LEN);
                // Receiving the request
                int msg_size;
                if ((msg_size = recv(sock_fd, &recvd_data, sizeof(st_request), 0)) <= 0)
                {
                    fprintf(stderr, REDB("recv  : %s\n"), strerror(errno));
                    exit(EXIT_FAILURE);
                }

                if (recvd_data.request_type == IMG_DATA)
                {
                    fwrite(recvd_data.data, 1, MAX_DATA_LEN, received_image);
                }
                else if (recvd_data.request_type == IMG_DATA_END)
                {
                    break;
                }
            }

            // Close the file and socket
            fclose(received_image);

            printf(GREEN("Image received successfully!\n"));
        }

        close(sock_fd);
    }

    if (close(client_sock_fd) < 0)
    {
        fprintf(stderr, REDB("close : failed to close the nfs socket!\n"));
        exit(EXIT_FAILURE);
    }
    return NULL;
}
