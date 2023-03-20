#include <stdio.h>
#include <winsock2.h>

#define MAX_CLIENTS 10

typedef struct
{
    SOCKET socket;
    struct sockaddr_in address;
    int addressLength;
} CLIENT;

CLIENT currClients[MAX_CLIENTS];
int numClients;

int addClient(SOCKET newClientSocket, struct sockaddr_in *clientAddress, int clientAddressLength)
{
    if (numClients < MAX_CLIENTS)
    {
        currClients[numClients++] = (CLIENT){newClientSocket, *clientAddress, clientAddressLength};
        return TRUE;
    }
    return FALSE;
}

void closeConnection(CLIENT client)
{
    closesocket(client.socket);
}

void removeClient(int index)
{
    if (index < numClients)
    {
        closeConnection(currClients[index]);
        for (int i = index; i < MAX_CLIENTS - 1; i++)
        {
            currClients[i] = currClients[i + 1];
        }
        numClients--;
    }
}

int sendMessage(CLIENT client, char *message)
{
    return send(client.socket, message, sizeof(message), 0x0);
}

void broadcast(char *message)
{
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (sendMessage(currClients[i], message) == SOCKET_ERROR)
        {
            printf("[!] Could not send message to client %i\n", i + 1);
        }
    }
}
