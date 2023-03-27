#include "../utils/config.h"

int initializeClientSocket(SOCKET *sock, const char *ipAddr, int port)
{
    struct sockaddr_in server;

    // Fill in server address information
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(ipAddr);
    server.sin_port = htons(port);

    // Connect to server
    int error = initializeSocket(sock, server, connect);
    if (error)
    {
        printf("Connection failed\n");
    }

    return error;
}

void serverHandler(SOCKET socket)
{
    int bytesReceived;
    char buffer[BUF_SIZE];
    while (TRUE)
    {
        bytesReceived = recv(socket, buffer, BUF_SIZE, 0x0);
        buffer[bytesReceived] = '\0';
        printf("Received: %s\n", buffer);
    }
    closesocket(socket);
    return;
}

int main(void)
{
    printf("[i] Initializing Client\n");
    const char *serverIp = getUserInput("Server IP Address: ", BUF_SIZE);
    // char *serverPort = getUserInput("Server Port:", BUF_SIZE);
    // int port = strtol(serverPort, NULL, 0xA);

    SOCKET clientSocket = createSocket(AF_INET, SOCK_STREAM, 0);
    int error_code = initializeClientSocket(&clientSocket, serverIp, PORT);
    printf("%s\n", serverIp);
    if (error_code)
    {
        printf("[E] Could not connect to %s on port %i\n", serverIp, PORT);
        printf("[i] Exiting...\n");
    }
    serverHandler(clientSocket);

    WSACleanup();
    return 0;
}
