#include <winsock2.h>
#include <windows.h>

#include "../utils/config.h"
#include "../utils/command.h"
#include "../utils/socket.h"
#include "../utils/userIO.h"
#include "../utils/encryption.h"

int initializeServer(SOCKET *socket, char *bindAddress, int port)
{
    if (!(1 < port && port < (1 << 16)))
    {
        printf("[E] Invalid port [%d] supplied", port);
        return SOCKET_ERROR;
    }
    printf("[i] Creating socket on %s:%d\n", bindAddress, port);

    struct sockaddr_in serverConfig;
    serverConfig.sin_family = AF_INET;
    serverConfig.sin_addr.s_addr = inet_addr(bindAddress);
    serverConfig.sin_port = htons(port);

    printf("[i] Initializing socket\n");
    if (initializeSocket(socket, serverConfig, bind) == SOCKET_ERROR)
    {
        printf("[E] Failed to initialize socket");
        return SOCKET_ERROR;
    }
    printf("[i] Successfully bound to %s:%i\n", bindAddress, port);

    if (listen(*socket, MAX_CLIENTS) == SOCKET_ERROR)
    {
        printf("[E] Failed to listen\n");
        return SOCKET_ERROR;
    }
    printf("[i] Listening for connections\n");

    // Sucessfully created listening socket
    return 0x0;
}

BOOLEAN getUserCommand(COMMAND *command)
{
    while (TRUE)
    {
        char *userCommand = getUserInput("[>] Command: ", BUF_SIZE);
        strip(userCommand);
        COMMAND cmd = encodeCommand(userCommand);
        if (cmd != INVALID_COMMAND)
        {
            *command = cmd;
            return strcmp(userCommand, QUIT) == 0;
        }
        printf("[!] Invalid Command Supplied\n");
    }
}

int sendCommand(SOCKET *socket, char *message)
{
    return send(*socket, message, strlen(message), 0x0);
}

int fetchData(SOCKET *socket, char buffer[BUF_SIZE])
{
    return recv(*socket, buffer, BUF_SIZE, 0x0);
}

void clientHandler(SOCKET clientSocket)
{
    char *URL;
    COMMAND command;
    BOOLEAN quit = FALSE;
    char message[BUF_SIZE];
    int expectedBuffer;
    while (!quit)
    {
        int commandSize = 0;
        quit = getUserCommand(&command);

        BOOLEAN special = isSpecial(command);
        if (special)
        {
            URL = getUserInput("[>] URL: ", BUF_SIZE - sizeof PREAMBLE_FMT);
            strip(URL);
            commandSize = strlen(URL);
        }
        snprintf(message, BUF_SIZE, PREAMBLE_FMT, command, commandSize);
        if (sendCommand(&clientSocket, message) == SOCKET_ERROR)
        {
            printf("[!] Could not send command\n");
        }
        if (special)
        {
            snprintf(message, BUF_SIZE, SPECIAL_FMT, command, commandSize, URL);
            if (sendCommand(&clientSocket, message) == SOCKET_ERROR)
            {
                printf("[!] Could not send URL\n");
            }
        }
        if (fetchData(&clientSocket, message) == SOCKET_ERROR)
        {
            printf("[!] Received Invalid Data\n");
        }
        parseCommand(message, &command, &expectedBuffer, URL, isSpecial(command));
        char result[expectedBuffer];
        if (fetchData(&clientSocket, result) == SOCKET_ERROR)
        {
            printf("[!] Invalid Response");
        }
        result[expectedBuffer] = '\0';
        encrypt(command, result, expectedBuffer, expectedBuffer);
        printf("%s\n", result);
    }
    if (quit)
    {
        printf("[i] Quit command received\n");
    }
}

int main(int argc, char *argv[])
{
    SOCKET serverSocket = createSocket(AF_INET, SOCK_STREAM, 0x0);

    // Parse bind address and port
    if (argc != 3)
    {
        printf("Invalid number of args: %i\n", argc);
        printf("Format: %s [HOST] [PORT]", argv[0]);
        return 1;
    }
    char *host = argv[1];
    int port = strtol(argv[2], NULL, 0xA);

    // Initialize server socket
    if (initializeServer(&serverSocket, host, port) == SOCKET_ERROR)
    {
        closesocket(serverSocket);
        WSACleanup();
        return 2;
    }

    SOCKET clientSocket;
    struct sockaddr_in clientAddr;
    int clientAddrLen = sizeof(clientAddr);
    // Currenting a 1-1 server-client relationship
    clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &clientAddrLen);
    printf("[i] New client %s:%d\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));
    clientHandler(clientSocket);

    WSACleanup();
    return 0;
}
