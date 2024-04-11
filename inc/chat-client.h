/*
* FILE : chat-client.h
* PROJECT : SENG2030 - Assignment #4
* PROGRAMMER : Anthony Phan
* FIRST VERSION : April 3, 2024
* DESCRIPTION :
* The functions in this file are used to file header for chat-client.c
*/

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <ncurses.h>
#include <signal.h>
#include <time.h>
#include <strings.h>
#include <signal.h>

#include "chat-network.h"
#include "chat-ui.h"

// prototypes 
void sigintHandler(int signum);
void displayMessage(const Message *message);
void *incomingHandler(void *arg);
void *outgoingHandler(void *arg);