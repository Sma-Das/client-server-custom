#include <pthread.h>

#include "../utils/config.h"

#define MAX_CLIENTS 10

int clientSockets[MAX_CLIENTS];
int currClients = 0;
pthread_mutex_t client_mutex;

int send_message(SOCKET clientSocket, char *command)
{
    // Send message
    return 0;
}

int broadcast(char *command)
{
    for (int i = 0; i < currClients; i++)
    {
        int error_code = send_message(clientSockets[i], command);
        if (error_code)
        {
            printf("[!] Error sending command to client %i", i);
        }
    }
}

int initializeServerSocket(SOCKET *sock, int port)
{
    struct sockaddr_in server_config;

    // Fill in server address information
    server_config.sin_family = AF_INET;
    server_config.sin_addr.s_addr = INADDR_ANY;
    server_config.sin_port = htons(port);

    // Connect to server
    int error = initializeSocket(sock, server_config, bind);
    if (error)
    {
        printf("Connection failed\n");
    }

    return error;
}

int main(void)
{

    SOCKET server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    HANDLE threads[MAX_CLIENTS];
    int num_threads = 0;

    // Initialize Winsock
    WSAStartup(MAKEWORD(2, 2), &wsa_data);

    // Create the server socket
    server_socket = createSocket(AF_INT, SOCK_STREAM, 0);
    initializeServerSocket(&server_socket, PORT);
    // Listen for connections
    listen(server_socket, MAX_CLIENTS);

    while (true)
    {
        SOCKET tempClientSocket =
    }
}