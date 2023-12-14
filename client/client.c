#include "client.h"

struct sockaddr_in client_address;       // IPv4 address struct for TCP communication between ss and nfs (requests)
int client_sock_fd;

int client_receiving_port_n = 0;
int credentials_saved = 0;
int logged_in = 0;
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

    // Getting the ip info
    my_ip = "0.0.0.0";

    // This code was commented by me as the ipinfo.io api was not working at that time
    // my_ip = get_my_ip();
    // if (my_ip == NULL)
    // {
    //     fprintf(stderr, RED("Could not get the ip address of the client\n"));
    //     exit(EXIT_FAILURE);
    // }

    printf(GREEN("Your ip is : %s\n"), my_ip);
    press_enter_to_contiue();

    while (1)
    {
        if (logged_in == 1)
        {
            menu();
        }
        else
        {
            // First try to read the credentials from the file, if not available then ask for the credentials
            FILE* fp = fopen("credentials.txt", "r");

            if (fp == NULL)
            {
                ask_signin_or_signup();
            }
            else
            {
                // Credentials file found
                credentials_saved = 1;
                // Read the credentials from the file
                memset(username, 0, sizeof(username));
                memset(password, 0, sizeof(password));
                fscanf(fp, "%s\n%s\n", username, password);
                fclose(fp);

                // Send signin request and see if the credentials saved are valid or not
                int result = send_signin_req();
                if (result != 0)
                {
                    printf(RESET_COLOR);
                    // If the signin fails it means the credentials stored are not right so just remove that file and ask for signin or signup
                    remove("credentials.txt");
                    ask_signin_or_signup();
                }
                else
                {
                    // Signin successful
                    logged_in = 1;
                }
            }
        }
    }

    return 0;
}

// Function to send the sign up request to the server, returns 0 if successful else 1
int send_signup_req()
{
    // Preparing the request to be sent
    st_request signup_req_st;
    memset(&signup_req_st, 0, sizeof(st_request));
    signup_req_st.request_type = SIGNUP;
    signup_req_st.recv_port_no = client_receiving_port_n;
    strcpy(signup_req_st.ip, my_ip);
    strcpy(signup_req_st.username, username);

    sprintf(signup_req_st.data, "%s", password); // <My password>

    int socket_fd = connect_to_server();

    // Sending the Sign Up request
    int sent_msg_size;
    if ((sent_msg_size = send(socket_fd, (request) &signup_req_st, sizeof(st_request), 0)) < 0)
    {
        fprintf(stderr, RED("send : could not send client Sign Up request : %s\n"), strerror(errno));
        return 1;
    }

    int recvd_msg_size;
    st_request signup_response_st;
    if ((recvd_msg_size = recv(socket_fd, (request) &signup_response_st, sizeof(st_request), 0)) < 0)
    {
        fprintf(stderr, RED("recv : could not recv client Sign Up request : %s\n"), strerror(errno));
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
        printf(RED("Sign Up failed!\n"));
        printf(RED("Reason: %s"), signup_response_st.data);
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
    memset(&signout_req_st, 0, sizeof(st_request));
    signout_req_st.request_type = SIGNOUT;
    signout_req_st.recv_port_no = client_receiving_port_n;
    strcpy(signout_req_st.ip, my_ip);
    strcpy(signout_req_st.username, username);

    int socket_fd = connect_to_server();

    // Sending the Sign Out request
    int sent_msg_size;
    if ((sent_msg_size = send(socket_fd, (request) &signout_req_st, sizeof(st_request), 0)) < 0)
    {
        fprintf(stderr, RED("send : could not send client Sign Out request : %s\n"), strerror(errno));
        return 1;
    }

    int recvd_msg_size;
    st_request signout_response_st;
    if ((recvd_msg_size = recv(socket_fd, (request) &signout_response_st, sizeof(st_request), 0)) < 0)
    {
        fprintf(stderr, RED("recv : could not recv client Sign Out request : %s\n"), strerror(errno));
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
        printf(RED("Sign Out failed!\n"));
        printf(RED("Reason: %s"), signout_response_st.data);
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
    memset(&signin_req_st, 0, sizeof(st_request));
    signin_req_st.request_type = SIGNIN;
    signin_req_st.recv_port_no = client_receiving_port_n;
    strcpy(signin_req_st.ip, my_ip);
    strcpy(signin_req_st.username, username);
    sprintf(signin_req_st.data, "%s", password); // <My password>
 
    int socket_fd = connect_to_server();

    // Sending the Sign In request
    int sent_msg_size;
    if ((sent_msg_size = send(socket_fd, (request) &signin_req_st, sizeof(st_request), 0)) < 0)
    {
        fprintf(stderr, RED("send : could not send client Sign In request : %s\n"), strerror(errno));
        return 1;
    }

    int recvd_msg_size;
    st_request signin_response_st;
    if ((recvd_msg_size = recv(socket_fd, (request) &signin_response_st, sizeof(st_request), 0)) < 0)
    {
        fprintf(stderr, RED("recv : could not recv client Sign In request : %s\n"), strerror(errno));
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
        printf(RED("Sign In failed!\n"));
        printf(RED("Reason: %s"), signin_response_st.data);
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
    memset(&delete_account_req_st, 0, sizeof(st_request));
    delete_account_req_st.request_type = DELETE_ACCOUNT;
    delete_account_req_st.recv_port_no = client_receiving_port_n;
    strcpy(delete_account_req_st.ip, my_ip);
    strcpy(delete_account_req_st.username, username);
    sprintf(delete_account_req_st.data, "%s", password);

    int socket_fd = connect_to_server();

    // Sending the Delete Account request
    int sent_msg_size;
    if ((sent_msg_size = send(socket_fd, (request) &delete_account_req_st, sizeof(st_request), 0)) < 0)
    {
        fprintf(stderr, RED("send : could not send client Delete Account request : %s\n"), strerror(errno));
        return 1;
    }

    int recvd_msg_size;
    st_request delete_account_response_st;
    if ((recvd_msg_size = recv(socket_fd, (request) &delete_account_response_st, sizeof(st_request), 0)) < 0)
    {
        fprintf(stderr, RED("recv : could not recv client Delete Account request : %s\n"), strerror(errno));
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
        printf(RED("Delete Account failed!\n"));
        printf(RED("Reason: %s"), delete_account_response_st.data);
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
            fprintf(stderr, RED("curl_easy_perform() failed: %s\n"), curl_easy_strerror(res));
            return NULL;
        } else {
            // Got the ip address
        }

        // Clean up
        curl_easy_cleanup(curl);
    } else {
        fprintf(stderr, RED("Failed to initialize curl\n"));
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
            fprintf(stderr, RED("fopen : could not open the credentials file : %s\n"), strerror(errno));
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
    memset(&signout_req_st, 0, sizeof(st_request));
    signout_req_st.request_type = SIGNOUT;
    signout_req_st.recv_port_no = client_receiving_port_n;
    strcpy(signout_req_st.ip, my_ip);
    strcpy(signout_req_st.username, username);

    int socket_fd = connect_to_server();

    int sent_msg_size;
    if ((sent_msg_size = send(socket_fd, (request) &signout_req_st, sizeof(st_request), 0)) < 0)
    {
        fprintf(stderr, RED("send : could not send client Sign Out request : %s\n"), strerror(errno));
    }

    int recvd_msg_size;
    st_request signout_response_st;
    if ((recvd_msg_size = recv(socket_fd, (request) &signout_response_st, sizeof(st_request), 0)) < 0)
    {
        fprintf(stderr, RED("recv : could not recv client Sign Out request ack : %s\n"), strerror(errno));
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
        fprintf(stderr, RED("socket : could not start server socket : %s\n"), strerror(errno));
        exit(EXIT_FAILURE);
    }

    client_address.sin_family = AF_INET;
    client_address.sin_port = htons(client_receiving_port_n);
    client_address.sin_addr.s_addr = INADDR_ANY;    // Any available address

    // Binding to the port
    if (bind(client_sock_fd, (struct sockaddr *) &client_address, sizeof(client_address)) == -1)
    {
        fprintf(stderr, RED("bind : could not bind server socket : %s\n"), strerror(errno));
        exit(EXIT_FAILURE);
    }

    // Listening for incoming requests for communication
    if (listen(client_sock_fd, MAX_PENDING) == -1)
    {
        fprintf(stderr, RED("listen : could not listen on server socket : %s\n"), strerror(errno));
        exit(EXIT_FAILURE);
    }

    printf(GREEN("\n==================== Started listening to incoming data ====================\n\n"));

    while (1)
    {
        // Keep accepting incoming requests
        struct sockaddr_in addr;
        int addr_size = sizeof(struct sockaddr_in);

        int sock_fd = accept(client_sock_fd, (struct sockaddr *) &addr, (socklen_t *)&addr_size);
        
        if (sock_fd == -1)
        {
            fprintf(stderr, RED("accept : could not accept connection on nfs socket : %s\n"), strerror(errno));
            exit(EXIT_FAILURE);
        }

        // Receiving the data
        st_request recvd_data;
        memset(&recvd_data, 0, sizeof(st_request));

        int recvd_msg_size;
        if ((recvd_msg_size = recv(sock_fd, &recvd_data, sizeof(st_request), 0)) <= 0)
        {
            fprintf(stderr, RED("recv  : %s\n"), strerror(errno));
            exit(EXIT_FAILURE);
        }

        // If the data sent is image data
        if (recvd_data.request_type == FILE_DATA)
        {
            receive_data(sock_fd, recvd_data, FILE_DATA, FILE_DATA_END);

            printf(GREEN("File received successfully!\n"));
            sleep(2);
            print_menu();
        }
        else if (recvd_data.request_type == IMG_DATA)
        {
            receive_data(sock_fd, recvd_data, IMG_DATA, IMG_DATA_END);
            printf(GREEN("Image received successfully!\n"));
            sleep(2);
            print_menu();
        }
        else if (recvd_data.request_type == MSG_DATA)
        {
            // Clear the terminal and print that we are receiving the message
            system(clear);
            printf("Receiving message from %s ...\n", recvd_data.send_from_username);

            printf(GREEN("Message: %s\n"), recvd_data.data);
            sleep(2);
            print_menu();
        }
        else if (recvd_data.request_type == AUDIO_DATA)
        {
            receive_data(sock_fd, recvd_data, AUDIO_DATA, AUDIO_DATA_END);

            play_audio("recorded_audio_received.wav");

            remove("recorded_audio_received.wav");
            sleep(1);
            print_menu();
        }
        else
        {
            // Clear the terminal and print that we are receiving the message
            system(clear);

            printf(RED("Unknown request received from %s\n"), recvd_data.send_from_username);
            sleep(2);
            print_menu();
        }

        if (close(sock_fd) < 0)
        {
            fprintf(stderr, RED("close : failed to close the socket!\n"));
            exit(EXIT_FAILURE);
        }
    }

    if (close(client_sock_fd) < 0)
    {
        fprintf(stderr, RED("close : failed to close the client socket!\n"));
        exit(EXIT_FAILURE);
    }
    return NULL;
}

// Function to print the menu
void print_menu()
{
    system(clear);
    printf(ORANGE("What do you want to do?\n"));
    printf(CYAN("1. Send Message\n"));
    printf(CYAN("2. Send File\n"));
    printf(CYAN("3. Send Image\n"));
    printf(CYAN("4. Signout\n"));
    printf(CYAN("5. Delete Account\n"));
    printf(CYAN("6. Send voice message\n"));
    printf(CYAN("7. Exit\n"));

    printf(YELLOW("\nEnter your choice: "));
    fflush(stdout);

    return;
}

// Checks if the user you want to send data to is online or not
int check_if_user_is_online(const int socket_fd, const int data_type, const char* to_username)
{
    st_request req_st;
    memset(&req_st, 0, sizeof(st_request));
    req_st.request_type = data_type;

    strcpy(req_st.send_from_username, username);
    strcpy(req_st.send_to_username, to_username);

    // Sending the Send Image request
    int sent_msg_size;
    if ((sent_msg_size = send(socket_fd, (request) &req_st, sizeof(st_request), 0)) < 0)
    {
        fprintf(stderr, RED("send : could not send client Send Image request : %s\n"), strerror(errno));
        press_enter_to_contiue();
        return -1;
    }

    // Receiving the response
    int recvd_msg_size;
    st_request response_st;
    if ((recvd_msg_size = recv(socket_fd, (request) &response_st, sizeof(st_request), 0)) < 0)
    {
        fprintf(stderr, RED("recv : could not recv client Send Image request : %s\n"), strerror(errno));
        press_enter_to_contiue();
        return -1;
    }

    return response_st.request_type;
}

// Asks the user if he/she wants to signin to an existing account or wants to create a new account
void ask_signin_or_signup()
{
    // Clear the terminal
    system(clear);

    // Asking user if he wants to signin or signup
    printf(PINK("What do you want to do?\n"));
    printf(PINK("1. Sign In\n"));
    printf(PINK("2. Sign Up\n"));

    printf(YELLOW("\nEnter your choice: "));
    int choice;
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
            logged_in = 1;

            // Now if the credentials are not saved then asking the user if he wants to save the credentials or not
            if (credentials_saved == 0)
                ask_to_save_credentials();
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
            logged_in = 1;

            // Asking if the user wants to save it's credentials
            ask_to_save_credentials();
        }
    }
    else
    {
        char temp[100];
        printf(RED("Invalid choice\n"));
        press_enter_to_contiue();
    }

    return;
}

// Prints the menu and then asks for the input
void menu()
{
    print_menu();

    int choice;
    scanf("%d", &choice);
    printf("\n");

    if (choice == 1)
    {
        // Send message
        printf("Enter the username of the person you want to send the message to: ");
        char to_username[1024] = {0};
        scanf("%s", to_username);

        int socket_fd = connect_to_server();

        int reply = check_if_user_is_online(socket_fd, FIND_USER, to_username);

        if (reply == ACK)
        {
            // User is online, we can send message
            printf("Enter the message (max 1024 characters): ");
            char message[1024] = {0};
            scanf(" %[^\n]", message);

            printf(GREEN("Sending message...\n"));

            st_request message_st;
            memset(&message_st, 0, sizeof(st_request));
            message_st.request_type = MSG_DATA;

            strcpy(message_st.send_from_username, username);
            strcpy(message_st.send_to_username, to_username);
            strcpy(message_st.data, message);

            int sent_msg_size;
            if ((sent_msg_size = send(socket_fd, (request) &message_st, sizeof(st_request), 0)) < 0)
            {
                fprintf(stderr, RED("send : could not send client Send Message request : %s\n"), strerror(errno));
                close(socket_fd);
                press_enter_to_contiue();
                return;
            }

            printf(GREEN("Message sent successfully!\n"));
            press_enter_to_contiue();
            return;
        }
        else
        {
            // Send message request failed
            printf(RED("Send message request failed!\n"));
            close(socket_fd);
            press_enter_to_contiue();
            return;
        }
    }
    else if (choice == 2)
    {
        // Send file
        printf("Enter the username of the person you want to send the file to: ");
        char to_username[1024] = {0};
        scanf("%s", to_username);

        int socket_fd = connect_to_server();

        int reply = check_if_user_is_online(socket_fd, FIND_USER, to_username);

        if (reply == ACK)
        {
            // User is online
            printf(GREEN("User to be sent file to found!\n"));

            // File can be sent
            printf("Enter the path of the file: ");
            char file_path[1024] = {0};
            scanf("%s", file_path);

            printf(GREEN("Sending file...\n"));

            send_file(file_path, socket_fd, to_username, FILE_DATA, FILE_DATA_END);

            printf(GREEN("File sent successfully!\n"));
            press_enter_to_contiue();
            return;
        }
    }
    else if (choice == 3)
    {
        // Send image
        printf("Enter the username of the person you want to send the image to: ");
        char to_username[1024] = {0};
        scanf("%s", to_username);

        int socket_fd = connect_to_server();

        int reply = check_if_user_is_online(socket_fd, FIND_USER, to_username);

        if (reply == ACK)
        {
            // User is online
            printf(GREEN("User to be sent image to found!\n"));

            // Image can be sent
            printf("Enter the path of the image: ");
            char image_path[1024] = {0};
            scanf("%s", image_path);

            printf(GREEN("Sending image...\n"));

            send_file(image_path, socket_fd, to_username, IMG_DATA, IMG_DATA_END);

            printf(GREEN("Image sent successfully!\n"));
            press_enter_to_contiue();
            return;
        }
        else
        {
            // Send Image request failed
            printf(RED("Send Image request failed!\n"));
            close(socket_fd);
            press_enter_to_contiue();
            return;
        }
    }
    else if (choice == 4)
    {
        // signout
        printf(ORANGE("Signing out...\n"));
        int result = send_signout_req();
        press_enter_to_contiue();
        if (result == 0)
        {
            logged_in = 0;
        }
        return;
    }
    else if (choice == 5)
    {
        // Delete account
        printf(RED("Are you sure you want to delete your account? (y/n): "));
        char temp[2] = {0};
        scanf("%s", temp);
        if (strcmp(temp, "y") == 0)
        {
            printf("Enter your password to confirm: ");
            char entered_password[1024];
            scanf("%s", entered_password);
            if (strcmp(entered_password, password) != 0)
            {
                printf(RED("Incorrect password\n"));
                press_enter_to_contiue();
                return;
            }

            int result = send_delete_account_req();
            if (result == 0)
            {
                // Account deleted succesfully
                remove("credentials.txt");
                logged_in = 0;
                credentials_saved = 0;
            }
            press_enter_to_contiue();
            return;
        }
        else
            return;
    }
    else if (choice == 6)
    {
        // Send voice message
        printf("Enter the username of the person you want to send the voice message to: ");
        char to_username[1024] = {0};
        scanf("%s", to_username);

        int socket_fd = connect_to_server();

        int reply = check_if_user_is_online(socket_fd, FIND_USER, to_username);

        if (reply == ACK)
        {
            // User is online
            printf(GREEN("User to be sent voice message to found!\n"));

            printf("Enter the duration of the voice message (in seconds): ");
            int duration;
            scanf("%d", &duration);

            // Specify the filename for recording
            char *filename = "recorded_audio.wav";

            // Use the system call to execute the rec command
            char command[100];
            sprintf(command, "rec -r 44100 -b 16 -c 1 -e signed-integer -t wav %s trim 0 %d", filename, duration);

            int result = system(command);

            if (result == 0) {
                printf(GREEN("Audio recording successful. File: %s\n"), filename);
            } else {
                fprintf(stderr, RED("Error recording audio\n"));
                return;
            }

            printf(GREEN("Sending audio file...\n"));

            send_file(filename, socket_fd, to_username, AUDIO_DATA, AUDIO_DATA_END);

            printf(GREEN("Audio file sent successfully!\n"));
            remove(filename);
            press_enter_to_contiue();
            return;
        }
    }
    else if (choice == 7)
    {
        // exit
        printf(ORANGE("Exiting...\n"));
        int result;
        while ((result = send_signout_req()) != 0)
        {
            ;   // Continue
        }
        printf(GREEN("Exitted!\n"));
        exit(EXIT_SUCCESS);
    }
    else
    {
        printf(RED("Invalid choice\n"));
        press_enter_to_contiue();
    }

    return;
}

// Function to send any type of file
void send_file(char* file_path, int socket_fd, char* to_username, int flag, int end_flag)
{
    int sent_msg_size;
    FILE* File = fopen(file_path, "rb");
    if (File == NULL)
    {
        fprintf(stderr, RED("fopen : could not open the file : %s\n"), strerror(errno));
        st_request failed_st;
        failed_st.request_type = FAIL;
        if ((sent_msg_size = send(socket_fd, (st_request*) &failed_st, sizeof(st_request), 0)) < 0)
        {
            fprintf(stderr, RED("send : could not send client Send File request : %s\n"), strerror(errno));
            exit(EXIT_FAILURE);
        }
        close(socket_fd);
        press_enter_to_contiue();
        return;
    }

    // Send the File data
    st_request File_data;
    memset(&File_data, 0, sizeof(st_request));
    File_data.request_type = flag;

    strcpy(File_data.send_from_username, username);
    strcpy(File_data.send_to_username, to_username);

    char** tkns = tokenize(file_path, '/');
    char* file_name_with_extension = NULL;

    int i = 0;
    while (tkns[i] != NULL)
    {
        file_name_with_extension = tkns[i];
        i++;
    }

    char** tkns2 = tokenize(file_name_with_extension, '.');
    char* name = tkns2[0];
    char* ext = tkns2[1];

    sprintf(File_data.data, "%s_received|%s", name, ext); // <File name>|<File extension>

    free_tokens(tkns2);
    free_tokens(tkns);

    if ((sent_msg_size = send(socket_fd, (request) &File_data, sizeof(st_request), 0)) < 0)
    {
        fprintf(stderr, RED("send : could not send client Send File request : %s\n"), strerror(errno));
        exit(EXIT_FAILURE);
    }
    
    printf("Sent File name packet\n");

    while(1)
    {
        memset(File_data.data, 0, MAX_DATA_LEN);
        int bytes_read = fread(File_data.data, 1, MAX_DATA_LEN, File);
        if (bytes_read <= 0)
        {
            break;
        }

        if ((sent_msg_size = send(socket_fd, (request) &File_data, sizeof(st_request), 0)) < 0)
        {
            fprintf(stderr, RED("send : could not send client Send File request : %s\n"), strerror(errno));
            exit(EXIT_FAILURE);
        }
        
        printf("Sent File packet\n");
    }

    // Send the end of File data
    st_request File_data_end;
    File_data_end.request_type = end_flag;

    if ((sent_msg_size = send(socket_fd, (request) &File_data_end, sizeof(st_request), 0)) < 0)
    {
        fprintf(stderr, RED("send : could not send client Send File request : %s\n"), strerror(errno));
        exit(EXIT_FAILURE);
    }

    fclose(File);

    return;
}

// Receives the file data from the server
void receive_data(int sock_fd, st_request recvd_data, int flag, int end_flag)
{
    char** tkns = tokenize(recvd_data.data, '|');
    char* file_name = tkns[0];
    char* file_ext = tkns[1];

    // Clear the terminal and print that we are receiving the image
    system(clear);
    printf("Receiving file %s from %s ...\n", file_name, recvd_data.send_from_username);

    // Create a file to save the image
    char file[1024] = {0};

    strcpy(file, file_name);
    strcat(file, ".");
    strcat(file, file_ext);

    free_tokens(tkns);

    FILE *received_file = fopen(file, "wb");
    if (!received_file)
    {
        fprintf(stderr, RED("Error opening file to save received file"));
        sleep(2);
        print_menu();
    }

    printf("Opened file\n");

    // Keep receving the file data until we get the end of file data
    while(1)
    {
        memset(recvd_data.data, 0, MAX_DATA_LEN);

        // Receiving the request
        int msg_size;
        if ((msg_size = recv(sock_fd, &recvd_data, sizeof(st_request), 0)) <= 0)
        {
            fprintf(stderr, RED("recv  : %s\n"), strerror(errno));
            exit(EXIT_FAILURE);
        }

        if (recvd_data.request_type == flag)
        {
            fwrite(recvd_data.data, 1, MAX_DATA_LEN, received_file);
        }
        else if (recvd_data.request_type == FAIL)
        {
            printf(RED("Failed to receive File!\n"));
            sleep(2);
            print_menu();
            break;
        }
        else if (recvd_data.request_type == end_flag)
        {
            break;
        }

        printf(GREEN("Received File packet\n"));
    }

    // Close the file and socket
    fclose(received_file);
}

// Plays the provided audio file
void play_audio(const char *filename)
{
    char command[256];
    sprintf(command, "afplay %s", filename);

    int result = system(command);

    if (result == 0)
    {
        printf("Audio playback successful.\n");
    } else
    {
        fprintf(stderr, "Error playing audio.\n");
    }
}
