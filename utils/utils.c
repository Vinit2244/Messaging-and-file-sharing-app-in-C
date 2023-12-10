#include "utils.h"

// Sends the mentioned acknowledgement to the given socket file descriptor
void send_ack(const int status_code, const int sock_fd, const char* msg)
{
    // Send acknowledgement
    st_request ack_st;
    ack_st.request_type = status_code;
    if (msg != NULL)
    {
        memset(ack_st.data, 0, MAX_DATA_LEN);
        strcpy(ack_st.data, msg);
    }

    // Nothing to be written onto the data as only ack is being sent
    int sent_msg_size;
    if ((sent_msg_size = send(sock_fd, (request) &ack_st, sizeof(st_request), 0)) <= 0)
    {
        fprintf(stderr, RED("send : could not sent acknowledgement for Request type %d : %s\n"), status_code, strerror(errno));
    }

    return;
}

// This function tokenises the provided string on given character and returns a 2D character array broken at ch
char **tokenize(const char *str, const char ch)
{
    // Counting the number of delimiters
    int num_of_ch = 0;
    for (int i = 0; i < strlen(str); i++)
    {
        if (str[i] == ch)
        {
            num_of_ch++;
        }
    }

    // Number of tokens would be 1 more than the number of delimiters present in the string
    int num_of_tokens = num_of_ch + 1;

    // Allocating num_of_tokens + 1 memory because we need to store last token as NULL token to mark the end of tokens
    char **tokens = (char **) malloc((num_of_tokens + 1) * sizeof(char*));
    for (int i = 0; i < num_of_tokens; i++)
    {
        tokens[i] = (char *) calloc(MAX_DATA_LEN, sizeof(char));
    }
    // The last token will be kept null so that when traversing we would know when the tokens end by checking for NULL token
    tokens[num_of_tokens] = NULL;

    int token_idx = 0;     // Index of the token being stored
    int token_str_idx = 0; // Index where the next character is to be stored on token
    for (int i = 0; i < strlen(str); i++)
    {
        // If the delimiter character is encountered increment the token index by 1 to start storing the next token and reset the token string index to 0 to start storing from the starting of the string
        if (str[i] == ch)
        {
            token_idx++;
            token_str_idx = 0;
            continue;
        }
        else
        {
            tokens[token_idx][token_str_idx++] = str[i];
        }
    }

    return tokens;
}

// Frees the memory allocated to the 2D tokens array
void free_tokens(char **tokens)
{
    // Looping through all the tokens untill the NULL token is encountered which marks the end of the tokens array
    int i = 0;
    while (tokens[i] != NULL)
    {
        free(tokens[i]);
        i++;
    }
    free(tokens);
    return;
}

// Waits for the user to press enter to continue
void press_enter_to_contiue()
{
    printf(GREEN("\nPress Enter to continue..."));
    fflush(stdout);             // Ensure the prompt is displayed before waiting for input
    getchar();                  // Wait for enter
    while (getchar() != '\n');  // Clear the input buffer
}

// Detects the user's OS and stores the clear command in the clear variable
void detect_os(char* clear)
{
    // Detecting the user's OS
    #if defined(_WIN32)
        strcpy(clear, "cls")
    #elif defined(__linux__)
        strcpy(clear, "clear");
    #elif defined(__APPLE__)
        strcpy(clear, "clear");
    #endif

    return;
}

// Connects to the server and returns the socket file descriptor
int connect_to_server()
{
    struct sockaddr_in address;
    memset(&address, 0, sizeof(address));

    int socket_fd = socket(PF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0) 
    { 
        // Some error occured while creating socket
        fprintf(stderr, RED("socket : could not create socket for client : %s\n"), strerror(errno));
        return -1;
    }

    address.sin_port    = htons(SERVER_PORT_N);        // port on which server side process is listening
    address.sin_family  = AF_INET;

    if (inet_pton(AF_INET, SERVER_IP, &address.sin_addr.s_addr) <= 0) 
    {   
        fprintf(stderr, RED("inet_pton : could not conver ip string to short int for client : %s\n"), strerror(errno));
        return -1;
    }

    while (1)
    {
        if (connect(socket_fd, (struct sockaddr *) &address, sizeof(address)) == -1) 
        {
            // Could not connect
            continue;
        }
        // Connected
        break;
    }

    return socket_fd;
}

// Connects to the client and returns the socket file descriptor
int connect_to_client(const char* ip, const int port)
{
    struct sockaddr_in address;
    memset(&address, 0, sizeof(address));

    int socket_fd = socket(PF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0) 
    { 
        // Some error occured while creating socket
        fprintf(stderr, RED("socket : could not create socket for client : %s\n"), strerror(errno));
        return -1;
    }

    address.sin_port    = htons(port);        // port on which server side process is listening
    address.sin_family  = AF_INET;

    if (inet_pton(AF_INET, ip, &address.sin_addr.s_addr) <= 0) 
    {   
        fprintf(stderr, RED("inet_pton : could not conver ip string to short int for client : %s\n"), strerror(errno));
        return -1;
    }

    while (1)
    {
        if (connect(socket_fd, (struct sockaddr *) &address, sizeof(address)) == -1) 
        {
            // Could not connect
            continue;
        }
        // Connected
        break;
    }

    return socket_fd;
}

