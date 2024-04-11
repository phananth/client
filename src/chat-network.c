/*
* FILE : chat-network.c
* PROJECT : SENG2030 - Assignment #4
* PROGRAMMER : Anthony Phan
* FIRST VERSION : April 3, 2024
* DESCRIPTION :
* The functions in this file are used for the networking of the client for the chat app. Used to connect and do things with the server.
*/

#include "../inc/chat-network.h"

// FUNCTION     : connectToServer
// DESCRIPTION  : used to connect to the server using the server ip and port
// PARAMETERS   : const char *serverIP, int port
// RETURNS      : my_server_socket
int connectToServer(const char *serverIP, int port) {
    // check to see if the serverip and port are valid
    if (serverIP == NULL || port <= 0) {
        printf("invalid server ip address or port\n");
        return -1;
    }
    
    int my_server_socket;
    struct sockaddr_in server_addr;
    struct hostent *host = gethostbyname(serverIP);

    // determine host info
    if (host == NULL) {
        printf("[CLIENT]: Host Info Search - FAILED\n");
        return -2;
    }

    // get socket for the server
    if ((my_server_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("CLIENT ERROR");
        return -3;
    }
    
    // initialize server address info for binding
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    memcpy(&server_addr.sin_addr, host->h_addr_list[0], host->h_length);
    server_addr.sin_port = htons(port);

    // attempt to connect to server
    if (connect(my_server_socket, (struct sockaddr *)&server_addr, sizeof (server_addr)) < 0) {
        printf("Error connecting to server");
        close(my_server_socket);
        return -4;
    }

    return my_server_socket;

}

// FUNCTION     : sendMessage
// DESCRIPTION  : used to send messages to the server
// PARAMETERS   : int socket, const Message *message
// RETURNS      : nothing
void sendMessage(int socket, const Message *message) {
    // set the length of the struct
    size_t userLength = strlen(message->username);
    size_t msgLength = strlen(message->messageText);

    // set chunk size to send
    int chunks = (msgLength + CHUNK_SIZE - 1) / CHUNK_SIZE;

    // write userlength to server
    if (write(socket, &userLength, sizeof(size_t)) < 0) {
        return;
    }

    // write username to server
    if (write(socket, message->username, userLength) < 0) {
        return;
    }

    // set temp empty ip - server's job to set the ip of the clients
    size_t ipLength = 0;

    // write ip to server
    if (write(socket, &ipLength, sizeof(size_t)) < 0) {
        return;
    }

    // write the message length to the server 
    if (write(socket, &msgLength, sizeof(size_t)) < 0) {
        return;
    }

    // send message in chunks
    size_t remaining = msgLength;
    const char *msgPtr = message->messageText;

    while (remaining > 0) {
        size_t chunkSize;
        if (remaining > CHUNK_SIZE) {
            chunkSize = CHUNK_SIZE;
        } else {
            chunkSize = remaining;
        }

        while (chunkSize < remaining && msgPtr[chunkSize] != ' ' && chunkSize < MAX_MESSAGE_LENGTH) {
            chunkSize++;
        }

        if (write(socket, msgPtr, chunkSize) < 0) {
            return;
        }

        msgPtr += chunkSize;
        remaining -= chunkSize;
    }
}

// FUNCTION     : receiveMessage
// DESCRIPTION  : used to receive messages from the server
// PARAMETERS   : int socket, Message *message
// RETURNS      : nothing
void receiveMessage(int socket, Message *message) {

    // receive message length
    ssize_t bytes;
    size_t total_length;
    bytes = recv(socket, &total_length, sizeof(size_t), 0);
    if (bytes <= 0) {
        return;
    }
    if (total_length >= sizeof(message->messageText)) {
        return;
    }

    // receive username length and username
    size_t userLength;
    bytes = recv(socket, &userLength, sizeof(size_t), 0);
    if (bytes <= 0) {
        return;
    }
    if (userLength >= sizeof(message->username)) {
        return;
    }
    bytes = recv(socket, message->username, userLength, 0);
    if (bytes <= 0) {
        return;
    }
    message->username[userLength] = '\0';

    // receive ip length and IP
    size_t ipLength;
    bytes = recv(socket, &ipLength, sizeof(size_t), 0);
    if (bytes <= 0) {
        return;
    }
    if (ipLength >= sizeof(message->senderIP)) {
        return;
    }
    bytes = recv(socket, message->senderIP, ipLength, 0);
    if (bytes <= 0) {
        return;
    }
    message->senderIP[ipLength] = '\0';

    // receive message length
    size_t msgLength;
    bytes = recv(socket, &msgLength, sizeof(size_t), 0);
    if (bytes <= 0) {
        return;
    }
    
    // receive all message chunks
    size_t remaining = msgLength;
    char *msgPtr = message->messageText;
    while (remaining > 0) {
        size_t chunkSize;
        if (remaining > CHUNK_SIZE) {
            chunkSize = CHUNK_SIZE;
        } else {
            chunkSize = remaining;
        }

        bytes = recv(socket, msgPtr, chunkSize, 0);
        if (bytes <= 0) {
            return;
        }

        msgPtr += bytes;
        remaining -= bytes;
    }
    message->messageText[msgLength]  = '\0';

}
