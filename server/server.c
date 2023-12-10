#include "server.h"

char* PWD;
pthread_t* requests_serving_threads_arr;
int* thread_slot_empty_arr;
int server_socket_fd;
struct sockaddr_in server_address;       // IPv4 address struct for TCP communication between ss and nfs (requests)
char clear[10];

int main(int argc, char* argv[])
{
    // Handling Ctrl + z (SIGTSTP) signal to print the log information on the screen without interrupting the working of Server
    struct sigaction sa;
    sa.sa_handler = &handleCtrlZ;  // Ctrl + Z (Windows/Linux/Mac)
    sa.sa_flags = SA_RESTART;      // Automatically restart the system call
    sigaction(SIGTSTP, &sa, NULL); // Ctrl + Z sends SIGTSTP signal (Signal Stop) - Prints the log onto the screen

    // Detecting the operating system of the server and setting the clear command accordingly
    detect_os(clear);

    // Clearing the terminal screen
    system(clear);

    PWD = (char*) calloc(MAX_LEN, sizeof(char));
    if (PWD == NULL)
    {
        fprintf(stderr, RED("calloc : cannot allocate memory to pwd char array : %s\n"), strerror(errno));
        exit(EXIT_FAILURE);
    }

    if (getcwd(PWD, MAX_LEN) == NULL)
    {
        fprintf(stderr, RED("pwd : error in getting pwd : %s\n"), strerror(errno));
        exit(EXIT_FAILURE);
    }

    pthread_t request_listening_thread;
    if (pthread_create(&request_listening_thread, NULL, &listening_thread, NULL) != 0)
    {
        fprintf(stderr, RED("pthread_create : Unable to create listening_thread : %s\n"), strerror(errno));
        exit(EXIT_FAILURE);
    }

    if (pthread_join(request_listening_thread, NULL) != 0)
    {
        fprintf(stderr, RED("pthread_join : Could not join thread listening_thread : %s\n"), strerror(errno));
        exit(EXIT_FAILURE);
    }
    
    return 0;
}
