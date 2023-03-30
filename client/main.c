#include <winsock2.h>
#include <windows.h>

#include "../utils/config.h"
#include "../utils/socket.h"
#include "../utils/userIO.h"
#include "../utils/command.h"
#include "../utils/encryption.h"
#include "../utils/logging.h"

int initializeClientSocket(SOCKET *clientSocket, const char *serverIp, int serverPort)
{
    struct sockaddr_in serverDetails;

    // Fill in server address information
    serverDetails.sin_family = AF_INET;
    serverDetails.sin_addr.s_addr = inet_addr(serverIp);
    serverDetails.sin_port = htons(serverPort);

    // Connect to server
    if (connect(*clientSocket, (struct sockaddr *)&serverDetails, sizeof(serverDetails)) < 0)
    {
        closesocket(*clientSocket);
        return 1;
    }
    return 0;
}

int fetchData(SOCKET *socket, char buffer[BUF_SIZE])
{
    return recv(*socket, buffer, BUF_SIZE, 0x0);
}

void serverHandler(SOCKET socket)
{
    COMMAND command;
    char buffer[BUF_SIZE], URL[BUF_SIZE], time[TIME_BUF_SIZ];
    int bytesReceived, expectedBuffer;
    while (TRUE)
    {
        bytesReceived = fetchData(&socket, buffer);
        if (bytesReceived == SOCKET_ERROR || !bytesReceived)
        {
            break;
        }
        buffer[bytesReceived] = '\0';
        parseCommand(buffer, &command, &expectedBuffer, NULL, FALSE);
        if (expectedBuffer > 0)
        {
            bytesReceived = fetchData(&socket, buffer);
            if (bytesReceived == SOCKET_ERROR || !bytesReceived)
            {
                break;
            }
            buffer[bytesReceived] = '\0';
            parseCommand(buffer, &command, &expectedBuffer, URL, TRUE);
        }
        char *commandName = decodeCommand(command);
        getCurrTime(time);
        printf("[%s] Received %s\n", time, commandName);
    }
    closesocket(socket);
    return;
}

int main(void)
{
    printf("[i] Initializing Client\n");

    char serverIp[BUF_SIZE], serverPort[BUF_SIZE];
    getInput("Server IP Address: ", serverIp);
    getInput("Server Port: ", serverPort);
    int port = strtol(serverPort, NULL, 0xA);

    strip(serverIp);
    printf("%s:%i\n", serverIp, port);
    SOCKET clientSocket = createSocket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET)
    {
        exit(10);
    }
    int error_code = initializeClientSocket(&clientSocket, serverIp, port);

    if (error_code)
    {
        printf("[E] Could not connect to %s on port %i\n", serverIp, port);
        printf("[i] Exiting...\n");
    }
    serverHandler(clientSocket);

    WSACleanup();
    return 0;
}
