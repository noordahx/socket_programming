#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>

// Default server addr, socket, buffer size (taken from server program)
#define DEFAULT_SERVER_ADDRESS "127.0.0.1"
#define DEFAULT_PORT 16111
#define BUFFER_SIZE 4096

void printClient(char *message) {
    printf("client: %s\n", message);
}

void printServer(char *message) {
    printf("server: %s\n", message);
}

void handlePost(SOCKET socket) {
    char message[BUFFER_SIZE];
    printClient("Enter your message (end with '#'):");

    send(socket, "POST", 4, 0);

    while (1) {
        fgets(message, sizeof(message), stdin);
        message[strcspn(message, "\n")] = 0;
        send(socket, message, strlen(message), 0);
        if (strcmp(message, "#") == 0) {
            char buffer[BUFFER_SIZE];
            memset(buffer, 0, BUFFER_SIZE);
            int bytesReceived = recv(socket, buffer, BUFFER_SIZE - 1, 0);
            if (bytesReceived <= 0) break;
            buffer[bytesReceived] = '\0';
            printServer(buffer);
            break;
        }
    }
}

void handleGet(SOCKET socket) {
    char buffer[BUFFER_SIZE];
    send(socket, "GET", 3, 0);

    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        int bytesReceived = recv(socket, buffer, BUFFER_SIZE - 1, 0);
        if (bytesReceived <= 0) break;

        buffer[bytesReceived] = '\0';
        if (strchr(buffer, '#')) {
            *strchr(buffer, '#') = '\0';
            printServer(buffer);
            break;
        }
        // printf("From Server: %s\n", buffer);
        printServer(buffer);
    }
}

void handleDelete(SOCKET socket) {
    char message_id[BUFFER_SIZE];
    // printf("Enter message IDs to delete (end with '#'):\n");
    // printClient("Enter message I")

    send(socket, "DELETE", 6, 0);

    while (1) {
        fgets(message_id, sizeof(message_id), stdin);
        message_id[strcspn(message_id, "\n")] = 0;

        send(socket, message_id, strlen(message_id), 0);
        if (strcmp(message_id, "#") == 0) {
            break;
        }
    }
}

int main(int argc, char *argv[]) {
    const char *serverAddress = DEFAULT_SERVER_ADDRESS;
    int serverPort = DEFAULT_PORT;

    if (argc > 2) {
        serverAddress = argv[1];
        serverPort = atoi(argv[2]);
    }

    WSADATA wsaData;
    SOCKET clientSocket;
    struct sockaddr_in serverAddr;

    if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0) {
        perror("WSAStartup failed");
        return 1;
    }

    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        perror("client: ERROR - Creating socket");
        WSACleanup();
        return 1;
    }

    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(serverPort);
    serverAddr.sin_addr.s_addr = inet_addr(serverAddress);
    // inet_pton(AF_INET, serverAddress, &serverAddr.sin_addr);
    if (serverAddr.sin_addr.s_addr == INADDR_NONE) {
        fprintf(stderr, "client: Invalid server address: %s\n", serverAddress);
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    if (connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        perror("client: Connection to server failed");
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    while (1) {
        char command[10];
        // printf("Enter a command (POST, GET, DELETE, QUIT): ");
        fgets(command, sizeof(command), stdin);
        command[strcspn(command, "\n")] = 0;

        for (char * p = command; *p; ++p) *p = toupper(*p);

        if (strcmp(command, "POST") == 0) {
            handlePost(clientSocket);
        } else if (strcmp(command, "GET") == 0) {
            handleGet(clientSocket);
        } else if (strcmp(command, "DELETE") == 0) {
            handleDelete(clientSocket);
        } else if (strcmp(command, "QUIT") == 0) {
            send(clientSocket, command, strlen(command), 0);
            break;
        } else {
            printClient("WRONG COMMAND");
        }
    }

    closesocket(clientSocket);
    WSACleanup();
    return 0;
}

