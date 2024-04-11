/*
* FILE : chat-client.c
* PROJECT : SENG2030 - Assignment #4
* PROGRAMMER : Anthony Phan
* FIRST VERSION : April 3, 2024
* DESCRIPTION :
* The functions in this file are used for the interactivity of the client for the chat app
*/

#include "../inc/chat-client.h"

// global variables
int client_socket;
char *username;
Message messageBuffer[MAX_HISTORY_LINES] = {0};
int messageStart = 0;
int messageCount = 0;
volatile sig_atomic_t clientShutdown = 0;

// FUNCTION     : sigintHandler
// DESCRIPTION  : used to signal out of the client. A way to exit the chat gracefully
// PARAMETERS   : int signum
// RETURNS      : nothing
void sigintHandler(int signum) {
    // set flag to shut down client
    clientShutdown = 1;

    // shut down gracefully
    cleanupNcurses();
    close(client_socket);
    exit(0);
}

// FUNCTION     : displayMessage
// DESCRIPTION  : used to display incoming messages in the messages window
// PARAMETERS   : const Message *message
// RETURNS      : nothing
void displayMessage(const Message *message) {
    // add receiving messages to a message buffer
    int index = (messageStart + messageCount) % MAX_HISTORY_LINES;
    messageBuffer[index] = *message;

    // check to see if message count is less than the max amount (10 messages)
    if (messageCount < MAX_HISTORY_LINES) {
        // increment message count
        messageCount++;
    } else {
        // update the index of the oldest message
        messageStart = (messageStart + 1) % MAX_HISTORY_LINES;
    }

    // erase incoming window
    werase(incoming_win);

    // get the incoming window dimensions
    int max_y, max_x;
    getmaxyx(incoming_win, max_y, max_x);
    int display = max_y - 2;

    // get the starting index to start displaying messages 
    int startIndex = messageStart;
    if (messageCount > display) {
        startIndex = (messageStart + messageCount - display) % MAX_HISTORY_LINES;
    }

    // display messages
    for (int i = 0; i < messageCount; i++) {
        int messageIndex = (startIndex + i) % MAX_HISTORY_LINES;
        Message *msg = &messageBuffer[messageIndex];

        // get the timestamp for when the message is received from the server
        struct tm *timeinfo;
        timeinfo = localtime(&msg->timestamp);

        // check to see if the person who sent the message is receiving their own message
        int isOutgoing = strcmp(msg->username, username) == 0;

        // check indicator based on incoming or outgoing message
        const char *indicator;
        if (isOutgoing) {
            // outgoing indicator
            indicator = ">>";
        } else {
            // incoming indicator
            indicator = "<<";
        }

        // display incoming formatted message 
        mvwprintw(incoming_win, i + 1, 1, "%-15s [%-5s] %s %-40s (%02d:%02d:%02d)\n", msg->senderIP, msg->username, indicator, msg->messageText,
            timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
    }

    // refresh incoming window
    box(incoming_win, 0, 0);
    wrefresh(incoming_win);
}

// FUNCTION     : incomingHandler
// DESCRIPTION  : used to handle incoming messages
// PARAMETERS   : void *arg
// RETURNS      : nothing
void *incomingHandler(void *arg) {
    int client_socket = *((int *)arg);
    Message message;

    // check to see if server is still running
    while (!clientShutdown) {
        // receive message from server
        receiveMessage(client_socket, &message);

        // display message
        displayMessage(&message);

        // keep cursor in the outgoing window
        wmove(outgoing_win, 1, 2);

        // refresh outgoing window
        wrefresh(outgoing_win);
    }
}

// FUNCTION     : outgoingHandler
// DESCRIPTION  : used to handle outgoing messages
// PARAMETERS   : void *arg
// RETURNS      : nothing
void *outgoingHandler(void *arg) {
    int client_socket = *((int *)arg);
    char msg[MAX_MESSAGE_LENGTH + 1];
    int msgLength = 0;

    // display > in outgoing window and refresh it
    mvwprintw(outgoing_win, 1, 1, ">");
    wrefresh(outgoing_win);

    // check to see if server is running
    while (!clientShutdown) {
        // read character from the outgoing window
        int ch = wgetch(outgoing_win);

        // check if the enter key is pressed
        if (ch == '\n') {
            msg[msgLength] = '\0';

            // check if the message is empty
            if (msgLength > 0) {

                // check if the message was the exit message
                if (strcmp(msg, ">>bye<<") == 0) {
                    if (clientShutdown) {
                        break;
                    }
                    
                    // clean up and exit gracefully
                    cleanupNcurses();
                    close(client_socket);
                    exit(0);
                }

                // init message struct
                Message message;
                strncpy(message.messageText, msg, sizeof(message.messageText));
                message.isOutgoing = 1;

                // send message to the server
                sendMessage(client_socket, &message);

                // resest the message length
                msgLength = 0;

                // move cursor back to outgoing window
                wmove(outgoing_win, 1, 2);
                wclrtoeol(outgoing_win);
                mvwprintw(outgoing_win, 1, 1, ">");
                box(outgoing_win, 0, 0);
                wrefresh(outgoing_win);
            }
            // check if the backspace is pressed
        } else if (ch == KEY_BACKSPACE || ch == 127) {
            // functional backspace - delete message in the outgoing window
            if (msgLength > 0) {
                msgLength--;
                mvwprintw(outgoing_win, 1, 2 + msgLength, " ");
                wmove(outgoing_win, 1, 2 + msgLength);
                wrefresh(outgoing_win);
            }
            // check to see if the message is less that the max amount
        } else if (msgLength < MAX_MESSAGE_LENGTH) {
            // build the message
            msg[msgLength++] = ch;
            mvwprintw(outgoing_win, 1, 2 + msgLength - 1, "%c", ch);
            wrefresh(outgoing_win);
        }
        // sleep
        usleep(10000);
    }
    // exit the thread if the client shuts down
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    // parse message for connecting to the server
    if ((argc != 3) || (strncmp(argv[1], "-user", 5) != 0) || (strncmp(argv[2], "-server", 7) != 0) || (strlen(argv[1]) < 6) || (strlen(argv[2]) < 14)) {
        // display useage command
        printf("Usage: chat-client -user<userID> -server<server name>\n");
        return 1;
    }

    // signal 
    signal(SIGINT, sigintHandler);

    // set username and serverIP for client from the command line arguments
    username = argv[1] + 5;
    char *serverIP = argv[2] + 7;

    // initialize ncurses and create display
    initNcurses();
    createWindows();

    // connect to the server using the server ip and pot 5000
    client_socket = connectToServer(serverIP, 5000);
    if (client_socket < 0) {
        printf("error creating client socket");
        cleanupNcurses();
        return 1;
    }

    // send username length upon connecting
    size_t username_length = strlen(username);
    if (send(client_socket, &username_length, sizeof(size_t), 0) < 0) {
        cleanupNcurses();
        close(client_socket);
        return 1;
    }
    // send username when connect to the server
    if (send(client_socket, username, username_length, 0) < 0) {
        cleanupNcurses();
        close(client_socket);
        return 1;
    }
    
    // create threads for handling outgoing messages
    pthread_t outgoing_tid, incoming_tid;
    if (pthread_create(&outgoing_tid, NULL, outgoingHandler, (void *)&client_socket) != 0) {
        cleanupNcurses();
        close(client_socket);
        return 1;
    }

    // create threads for handling incoming messages
    if (pthread_create(&incoming_tid, NULL, incomingHandler, (void *)&client_socket) != 0) {
        cleanupNcurses();
        close(client_socket);
        return 1;
    }

    // join the threads
    pthread_join(outgoing_tid, NULL);
    pthread_join(incoming_tid, NULL);

    // clean up the ncurses and close the socket
    cleanupNcurses();
    close(client_socket);

    return 0;

}
