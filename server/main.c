#include "../utils/config.h"

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

    if (listen(*socket, 1) == SOCKET_ERROR)
    {
        printf("[E] Failed to listen");
        return SOCKET_ERROR;
    }
    printf("[i] Listening for connections ");
    return 0x0;
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

    while (1)
    {
    }

    WSACleanup();
    return 0;
}
