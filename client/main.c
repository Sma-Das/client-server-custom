#include "../utils/config.h"

int initializeClientSocket(SOCKET *sock, char *ipAddr, int port)
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

int main(void)
{
    printf("[i] Initializing Client\n");
    char *serverIp = getUserInput("Server IP Address: ", BUF_SIZE);

    SOCKET client_socket = createSocket(AF_INET, SOCK_STREAM, 0);
    int error_code = initializeClientSocket(&client_socket, serverIp, PORT);
    if (error_code)
    {
        printf("[E] Could not connect to %s on port %i\n", serverIp, PORT);
        printf("[i] Exiting...\n");
    }
}
