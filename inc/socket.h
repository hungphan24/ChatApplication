#ifndef SOCKET_H
#define SOCKET_H

#include <stdio.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <errno.h> 

#define     MAX_CLIENT_SOCKET  10
#define     MAX_BUFFER_SIZE    1024
 
static int client_socket[10];
static int port = 0;
static int master_socket;
static struct sockaddr_in address;
static fd_set readfds;

void displayMyIP();
void *socketHandler(void *_port);
void connectToNewSocket(char* ipaddress, int port);


#endif  // SOCKET_H