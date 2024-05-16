#include "socket.h"
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <ifaddrs.h>
#define _POSIX_C_SOURCE 200809L

static void initClientsSocket() {
    for (int i = 0; i < MAX_CLIENT_SOCKET; i++)   
    {   
        client_socket[i] = 0;   
    }

    for (int i = 0; i < MAX_CLIENT_SOCKET; i++)   
    {   
        server_socket[i] = 0;   
    }  
}

static void createMasterSocket() {
    int opt = 1;
    //create a master socket  
    if( (master_socket = socket(AF_INET , SOCK_STREAM , 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    if( setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt,  
          sizeof(opt)) < 0 ) {
        perror("setsockopt");   
        exit(EXIT_FAILURE);   
    }

    //type of socket created  
    address.sin_family = AF_INET;   
    address.sin_addr.s_addr = INADDR_ANY;   
    address.sin_port = htons( port );

    if (bind(master_socket, (struct sockaddr *)&address, sizeof(address))<0)   
    {   
        perror("bind failed");   
        exit(EXIT_FAILURE);   
    }   
    printf("App running on port %d \n", port); 
    //try to specify maximum of 3 pending connections for the master socket  
    if (listen(master_socket, 3) < 0)   
    {   
        perror("listen");   
        exit(EXIT_FAILURE);   
    }   
}

static void addClientSocketToArray(int new_socket) {
    for (int i = 0; i < MAX_CLIENT_SOCKET; i++) {   
        //if position is empty  
        if( client_socket[i] == 0 ) {   
            client_socket[i] = new_socket;  
            break;   
        }   
    }   
}

static void addServerSocketToArray(int new_socket) {
    for (int i = 0; i < MAX_CLIENT_SOCKET; i++) {   
        //if position is empty  
        if( server_socket[i] == 0 ) {   
            server_socket[i] = new_socket;  
            break;   
        }   
    }   
}

void displayMyPort() {
    static int myPort = 0;
    if(0 == myPort) {
        int addrlen = sizeof(address);
        getpeername(master_socket , (struct sockaddr*)&address , (socklen_t*)&addrlen);
        myPort = ntohs(address.sin_port);   
    }
    printf("My Port = %d\n", myPort);
}

void displayMyIP() {
    struct ifaddrs *ifaddr, *ifa;
    int family, s;
    char host[20];

    if (getifaddrs(&ifaddr) == -1) {
        perror("getifaddrs");
        exit(EXIT_FAILURE);
    }

    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == NULL) {
            continue;
        }

        family = ifa->ifa_addr->sa_family;

        if (family == AF_INET) {
            s = getnameinfo(ifa->ifa_addr,
                            sizeof(struct sockaddr_in),
                            host, NI_MAXHOST,
                            NULL, 0, NI_NUMERICHOST);
            if (s != 0) {
                printf("getnameinfo() failed: %s\n", gai_strerror(s));
                exit(EXIT_FAILURE);
            }
            if (strcmp(host, "127.0.0.1") != 0) {
                printf("Interface: %s\tAddress family: %s\tAddress: %s\n",
                        ifa->ifa_name,
                        (family == AF_INET) ? "AF_INET" : "AF_INET6",
                        host);
            }
        }
    }

    freeifaddrs(ifaddr);
}


void connectToNewSocket(char* ipaddress, int portNumber) {
    int server_fd;
    int status;
    struct sockaddr_in serv_addr;
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        exit(EXIT_FAILURE);
    }
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(portNumber);

    if (inet_pton(AF_INET, ipaddress, &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        exit(EXIT_FAILURE);
    }

    if ((status = connect(server_fd, (struct sockaddr*)&serv_addr,
                   sizeof(serv_addr))) < 0) {
        printf("\nConnection Failed \n");
        exit(EXIT_FAILURE);
    }
    addServerSocketToArray(server_fd);
    printf("\nConnect successfully\n");
}

void *socketHandler(void *_port) {
    
    int addrlen , new_socket , activity, valread , sd;   
    int max_sd;
    char buffer[MAX_BUFFER_SIZE + 1];

    initClientsSocket();
    port = *((int*)_port);
    createMasterSocket();
    
         
    //accept the incoming connection  
    addrlen = sizeof(address);
    while(1) {
        //clear the socket set  
        FD_ZERO(&readfds);   
     
        //add master socket to set  
        FD_SET(master_socket, &readfds);   
        max_sd = master_socket;

        // Add server sockets to set
        for (int i = 0; i < MAX_CLIENT_SOCKET; i++) {
            sd = server_socket[i];
            if (sd > 0) {
                FD_SET(sd, &readfds);
            }
            if (sd > max_sd) {
                max_sd = sd;
            }
        }

        //add child sockets to set  
        for ( int i = 0 ; i < MAX_CLIENT_SOCKET; i++) {   
            //socket descriptor  
            sd = client_socket[i];   
                 
            //if valid socket descriptor then add to read list  
            if(sd > 0)   
                FD_SET( sd , &readfds);   
                 
            //highest file descriptor number, need it for the select function  
            if(sd > max_sd)   
                max_sd = sd;   
        }
 
        activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL);   
       
        if ((activity < 0) && (errno != EINTR)) {   
            printf("select error");   
        }

        if (FD_ISSET(master_socket, &readfds)) {
            if ((new_socket = accept(master_socket,  
                    (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) {   
                perror("accept");   
                exit(EXIT_FAILURE);   
            }

            printf("New connection , socket fd is %d , ip is : %s , port : %d \n" , new_socket , inet_ntoa(address.sin_addr) , ntohs 
                  (address.sin_port));
            //add new socket to array of sockets  
            addClientSocketToArray(new_socket);
        }

        // Handle communication from connected servers
        for (int i = 0; i < MAX_CLIENT_SOCKET; i++) {
            sd = server_socket[i];

            if (FD_ISSET(sd, &readfds)) {
                if ((valread = read(sd, buffer, MAX_BUFFER_SIZE)) == 0) {
                    // Server disconnected
                    getpeername(sd , (struct sockaddr*)&address , (socklen_t*)&addrlen); 
                    printf("Server %s:%d disconnected\n", inet_ntoa(address.sin_addr), ntohs(address.sin_port));
                    close(sd);
                    server_socket[i] = 0;
                } else {
                    // Print message from server
                    buffer[valread] = '\0';
                    printf("Message from App%d: %s\n", i, buffer);
                }
            }
        }

        for (int i = 0; i < MAX_CLIENT_SOCKET; i++) {
            sd = client_socket[i];
            if (FD_ISSET( sd , &readfds)) {
                getpeername(sd , (struct sockaddr*)&address , (socklen_t*)&addrlen); 
                if((valread = read( sd , buffer, MAX_BUFFER_SIZE)) == 0) {  
                    printf("Host disconnected , ip %s , port %d \n" ,  
                            inet_ntoa(address.sin_addr) , ntohs(address.sin_port));
                    close(sd);
                    client_socket[i] = 0;
                } else {
                    buffer[valread] = '\0';
                    printf("\n Message from App%d: ", i);
                    printf("%s\n", buffer);
                }
            }           
        }
    }
}

void terminateSocket(int connectionId) {
    if(connectionId == -1) {
        close(master_socket);
        return;
    }
    close(connectionId);
}

void sendMessage(int connectionId, char message[]) {
    send(connectionId, message, strlen(message), 0);
}

void listConnection() {
    printf("\nID\n");
    for (int i = 0; i < MAX_CLIENT_SOCKET; i++) {
        if(client_socket[i] != 0) {
            printf("%d     App%d\n", client_socket[i], client_socket[i]);
        }
    }
    for (int i = 0; i < MAX_CLIENT_SOCKET; i++) {
        if(server_socket[i] != 0) {
            printf("%d     App%d\n", server_socket[i], server_socket[i]);
        }
    }
}
