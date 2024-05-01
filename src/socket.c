#include "socket.h"

static void initClientsSocket() {
    for (int i = 0; i < MAX_CLIENT_SOCKET; i++)   
    {   
        client_socket[i] = 0;   
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

static void addNewSocketToArray(int new_socket) {
    for (int i = 0; i < MAX_CLIENT_SOCKET; i++) {   
        //if position is empty  
        if( client_socket[i] == 0 ) {   
            client_socket[i] = new_socket;  
            break;   
        }   
    }   
}

void displayMyIP() {
        
}

void connectToNewSocket(char* ipaddress, int portNumber) {
    int client_fd;
    int status;
    struct sockaddr_in serv_addr;
    if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        exit(EXIT_FAILURE);
    }
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(portNumber);

    if (inet_pton(AF_INET, ipaddress, &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        exit(EXIT_FAILURE);
    }

    if ((status = connect(client_fd, (struct sockaddr*)&serv_addr,
                   sizeof(serv_addr))) < 0) {
        printf("\nConnection Failed \n");
        exit(EXIT_FAILURE);
    }
    addNewSocketToArray(client_fd);
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
            addNewSocketToArray(new_socket);
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


