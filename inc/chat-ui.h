/*
* FILE : chat-ui.h
* PROJECT : SENG2030 - Assignment #4
* PROGRAMMER : Anthony Phan
* FIRST VERSION : April 3, 2024
* DESCRIPTION :
* The functions in this file are used to initialize the user interface for the chat app. File header for chat-ui.c
*/

#include <ncurses.h>
#include <string.h>

extern WINDOW *outgoing_win, *incoming_win;

// protoypes 
void initNcurses();
void createWindows();
void cleanupNcurses();