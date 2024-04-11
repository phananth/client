/*
* FILE : chat-network.h
* PROJECT : SENG2030 - Assignment #4
* PROGRAMMER : Anthony Phan
* FIRST VERSION : April 3, 2024
* DESCRIPTION :
* The functions in this file are used for networking to the server. File header for chat-network.c
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <time.h>

#include "../../chat-server/inc/message.h"

// constants
#define MAX_USERNAME_LENGTH 5
#define MAX_MESSAGE_LENGTH 80
#define MAX_HISTORY_LINES 10
#define CHUNK_SIZE 40

// prototypes
int connectToServer(const char *serverIP, int port);
void sendMessage(int socket, const Message *message);
void receiveMessage(int socket, Message *message);