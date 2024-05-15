#include "inc/socket.h"
#include "inc/Menu.h"
#include <pthread.h>
#include <string.h>
#include <arpa/inet.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>

#define MAX_LENGTH_COMMAND 100
#define MAX_LENGTH_MESSAGE 100

void parseInput(int argc, char const* argv[], int *port) {
    if (argc < 2) {
        printf("Port number is empty. Please add port number\n");
        exit(1);
    } else {
        *port = atoi(argv[1]);
    }
}

void sigintHandler(int sig) {
    terminateSocket(-1);
}

int main(int argc, char const* argv[]) {
    int shouldExit = 1;
    signal(SIGINT, sigintHandler);
    pthread_t thread_id1;
    char command[MAX_LENGTH_COMMAND];
    int port;
    parseInput(argc, argv, &port);
    displayMenu();

    pthread_create(&thread_id1, NULL, socketHandler, (void*) &port);
    while (shouldExit) {
        printf("Enter command: ");
        if (fgets(command, sizeof(command), stdin) == NULL) {
            continue;
        }
        command[strcspn(command, "\n")] = '\0';

        // Tokenize the command
        char *token = strtok(command, " ");
        if (token == NULL) {
            printf("Invalid command\n");
            continue;
        }

        printf("Command: %s\n", token); // Debugging output

        // Compare the command and call the appropriate function
        if (strcmp(token, "myip") == 0) {
            displayMyIP();
        } else if (strcmp(token, "myport") == 0) {
            displayMyPort();
        } else if (strcmp(token, "list") == 0) {
            listConnection();
        } else if (strcmp(token, "terminate") == 0) {
            token = strtok(NULL, " ");
            if (token == NULL) {
                printf("Invalid command format (missing connection ID)\n");
                continue;
            }
            int connectionId = atoi(token);
            terminateSocket(connectionId);

        } else if (strcmp(token, "connect") == 0) {
            token = strtok(NULL, " ");
            if (token == NULL) {
                printf("Invalid command format (missing IP address)\n");
                continue;
            }
            char ip[16];
            strncpy(ip, token, sizeof(ip));
            ip[sizeof(ip) - 1] = '\0';

            token = strtok(NULL, " ");
            if (token == NULL) {
                printf("Invalid command format (missing port number)\n");
                continue;
            }
            char *endptr;
            int port1 = strtol(token, &endptr, 10);
            if (*endptr != '\0') {
                printf("Invalid port number\n");
                continue;
            }
            printf("IP: %s, Port: %d\n", ip, port1);

            connectToNewSocket(ip, port1);
        } else if (strcmp(token, "send") == 0) {
            token = strtok(NULL, " ");
            if (token == NULL) {
                printf("Invalid command format (missing connection ID)\n");
                continue;
            }
            int connectionId = atoi(token);

            token = strtok(NULL, " ");
            if (token == NULL) {
                printf("Invalid command format (missing message)\n");
                continue;
            }
            char message[MAX_LENGTH_MESSAGE];
            strncpy(message, token, sizeof(message));
            message[sizeof(message) - 1] = '\0';
            sendMessage(connectionId, message);
        } else if (strcmp(token, "exit") == 0) {
            shouldExit = 0;
        } else {
            printf("Invalid command\n");
        }
    }

    return 0;
}
