#include "logging.h"

// Signal handler function
void handleCtrlZ(int signum)
{
    printf("\n");
    
    // Print all the logs
    FILE* fptr = fopen("logs.txt", "r");
    if (fptr == NULL)
    {
        fprintf(stderr, RED("fopen : %s\n"), strerror(errno));
        exit(EXIT_FAILURE);
    }

    char buffer[BUFFER_SIZE] = {0};

    // Read and print the file in chunks of BUFFER_SIZE bytes
    printf(YEL_COLOR);
    size_t bytesRead;
    while ((bytesRead = fread(buffer, 1, BUFFER_SIZE, fptr)) > 0)
    {
        // Process the buffer, e.g., print its content
        fwrite(buffer, 1, bytesRead, stdout);
    }
    printf(RESET_COLOR);

    fclose(fptr);
}

int insert_log(const int ss_or_client_port, const int request_type, const char* request_data, const int status_code, const char* username, const char* ip)
{
    FILE* fptr = fopen("logs.txt" , "a");
    if (fptr == NULL)
    {
        fprintf(stderr, RED("fopen : %s\n"), strerror(errno));
        return 0;
    }

    fprintf(fptr, "Communicating with Client\n");
    fprintf(fptr, "Client Port number                : %d\n", ss_or_client_port);
    fprintf(fptr, "Client Username                   : %s\n", username);
    fprintf(fptr, "Client IP                         : %s\n", ip);
    fprintf(fptr, "Request type                      : %d\n", request_type);
    fprintf(fptr, "Request data                      : %s\n", request_data);
    fprintf(fptr, "Status                            : %d\n", status_code);
    fprintf(fptr, "\n");

    fclose(fptr);

    return 1;
}

