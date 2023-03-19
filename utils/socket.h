#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

SOCKET create_socket(int af, int type, int protocol)
{
    WSADATA wsa;
    SOCKET sock;

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    {
        printf("WSAStartup failed\n");
        exit(1);
    }

    // Create socket
    sock = socket(af, type, protocol);

    // Check for errors
    if (sock == INVALID_SOCKET)
    {
        printf("Socket creation failed\n");
        WSACleanup();
        exit(1);
    }

    return sock;
}

int initializeSocket(SOCKET *sock, char *ipAddr, int port)
{
    struct sockaddr_in server;

    // Fill in server address information
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(ipAddr);
    server.sin_port = htons(port);

    // Connect to server
    if (connect(*sock, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR)
    {
        printf("Connection failed\n");
        return 1;
    }

    return 0;
}
