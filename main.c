#include "inc/socket.h"
#include "inc/Menu.h"
#include <pthread.h>
#include <string.h>
#include <arpa/inet.h>
#include<signal.h>

#define     MAX_LENGTH_COMMAND  20
#define     MAX_LENGTH_MESSAGE  20

void parseInput(int argc, char const* argv[], int *port) {
    if(argc < 2) {
        printf("Port number is empty. Please add port number\n");
        exit(1);
    } else {
        *port = atoi(argv[1]);
    }
}

void sigintHandler(int sig) {
    terminateSocket(-1);
}

int main(int argc , char const* argv[]) {
    int shouldExit = 1;
    signal(SIGINT, sigintHandler);
    pthread_t thread_id1;
    char command[MAX_LENGTH_COMMAND];
    int port;
     parseInput(argc, argv, &port);
    displayMenu();

    pthread_create(&thread_id1, NULL, socketHandler, (void*) &port);
    while(shouldExit){
        fgets(command, sizeof(command), stdin);
        command[strcspn(command, "\n")] = '\0';

        // Tokenize the command
        char *token = strtok(command, " ");
        if (token == NULL) {
            printf("Invalid command0\n");
            continue;
        }

        // Compare the command and call appropriate function
        if (strcmp(token, "myip") == 0) {
            displayMyIP();   
        } else if (strcmp(token, "myport") == 0) {
            displayMyPort();
        } else if (strcmp(token, "list") == 0) {
            listConnection();
        } else if (strcmp(token, "terminate") == 0) {
            int connectionId;
            token = strtok(NULL, " ");
            if (token == NULL) {
                printf("Invalid command format2\n");
                continue;
            }
            connectionId = atoi(token);
            terminateSocket(connectionId);
            
        } else if (strncmp(token, "connect", 7) == 0) {
            char ip[16];
            int port1;

            token = strtok(NULL, " ");
            if (token == NULL) {
                printf("Invalid command format1\n");
                continue;
            }
            strncpy(ip, token, sizeof(ip));
            ip[sizeof(ip) - 1] = '\0';

            token = strtok(NULL, " ");
            if (token == NULL) {
                printf("Invalid command format2\n");
                continue;
            }
            port1 = atoi(token);
            printf("IP: %s, Port: %d\n", ip, port1);

            connectToNewSocket(ip, port1);
        } else if (strncmp(token, "send", 7) == 0) {
            int connectionId;
            token = strtok(NULL, " ");
            char message[MAX_LENGTH_MESSAGE];
            if (token == NULL) {
                printf("Invalid command format2\n");
                continue;
            }
            connectionId = atoi(token);
            token = strtok(NULL, " ");
            if (token == NULL) {
                printf("Invalid command format1\n");
                continue;
            }
            strncpy(message, token, sizeof(message));
            message[sizeof(message) - 1] = '\0';
            // printf("%d %s\n", connectionId, message);
            sendMessage(connectionId, message);
        } else if (strcmp(token, "exit") == 0) {
            shouldExit = 0;
        } else {
            printf("Invalid command4\n");
        }
    }


    return 0;
}