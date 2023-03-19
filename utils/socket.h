#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>

typedef int (*CallbackFunc)(SOCKET, const struct sockaddr *, int);

#if defined(_MSC_VER) || defined(__BORLANDC__)
#pragma comment(lib, "Ws2_32.lib")
#endif

SOCKET createSocket(int af, int type, int protocol)
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

int initializeSocket(SOCKET *sock, struct sockaddr_in config, CallbackFunc startSocket)
{
    if (startSocket(*sock, (struct sockaddr *)&config, sizeof(config)) == SOCKET_ERROR)
    {
        return SOCKET_ERROR;
    }
    return 0;
}
