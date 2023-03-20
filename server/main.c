#include "../utils/config.h"
#include "./clientSocket.h"

int initializeServerSocket(SOCKET *sock, char *bindAddress, int port)
{
    struct sockaddr_in server_config;

    // Fill in server address information
    server_config.sin_family = AF_INET;
    server_config.sin_addr.s_addr = inet_addr(bindAddress);
    server_config.sin_port = htons(port);

    if (initializeSocket(sock, server_config, bind) == SOCKET_ERROR)
    {
        printf("[!] Error binding socket");
        return SOCKET_ERROR;
    }
    printf("[i] Socket bound to %s:%i\n", bindAddress, port);

    // Listen for connections
    printf("[i] Listening for connections...\n");
    int result = listen(*sock, MAX_CLIENTS);
    if (result == SOCKET_ERROR)
    {
        printf("Listen failed: %d\n", WSAGetLastError());
        closesocket(*sock);
        WSACleanup();
        return SOCKET_ERROR;
    }

    return 0;
}

int main(void)
{

    SOCKET serverSocket;
    HANDLE threads[MAX_CLIENTS];
    int num_threads = 0;

    // Create the server socket
    serverSocket = createSocket(AF_INET, SOCK_STREAM, 0);
    initializeServerSocket(&serverSocket, HOST, PORT);

    while (1)
    {
        struct sockaddr clientAddr;
        int clientAddrLen;
        SOCKET newClient = accept(serverSocket, &clientAddr, &clientAddrLen);
        if (newClient == INVALID_SOCKET)
        {
            printf("Accept failed: %d\n", WSAGetLastError());
            closesocket(serverSocket);
            WSACleanup();
            return 1;
        }
        printf("Client connected!\n");
        if (!addClient(newClient, (struct sockaddr_in *)&clientAddr, clientAddrLen))
        {
            printf("[!] Failed to add client");
        }

        // Handle the connection here...
    }
    closesocket(serverSocket);
    WSACleanup();
    return 0;
}
