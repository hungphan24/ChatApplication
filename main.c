#include "inc/socket.h"
#include "inc/Menu.h"
#include <pthread.h>

#define     MAX_LENGTH_COMMAND  20

void parseInput(int argc, char const* argv[], int *port) {
    if(argc < 2) {
        printf("Port number is empty. Please add port number\n");
    } else {
        *port = atoi(argv[1]);
    }
}

int main(int argc , char const* argv[]) {
    pthread_t thread_id1;
    char command[MAX_LENGTH_COMMAND];
    int port;
    parseInput(argc, argv, &port);
    displayMenu();

    pthread_create(&thread_id1, NULL, socketHandler, (void*) &port);
    while(1){
        fgets(command, sizeof(command), stdin);
    }


    return 0;
}