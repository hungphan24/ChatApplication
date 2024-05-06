#ifndef MENU_H
#define MENU_H

#include<stdio.h>

void displayMenu() {
    printf("********************************* Chat application *************************\n");
    printf("Command: \n");
    printf("1. myip\n");
    printf("2. myport\n");
    printf("3. connect <ip destination> <port> \n");
    printf("4. list\n");
    printf("5. terminate <connection id>\n");
    printf("6. send <connection id> <message>\n");
    printf("----------------------------------------------------------------------------\n");
}

#endif  // MENU_H