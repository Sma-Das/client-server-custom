#include <windows.h>
#include <psapi.h>
#include <stdio.h>
#include <string.h>

#include "config.h"

char *getProcesses(char *URL)
{
    DWORD processIds[128];
    DWORD bytesReturned;
    if (!EnumProcesses(processIds, sizeof(processIds), &bytesReturned))
    {
        fprintf(stderr, "Error: EnumProcesses failed with error %lu\n", GetLastError());
        return NULL;
    }
    int count = bytesReturned / sizeof(DWORD);

    char *buffer = (char *)malloc(BUF_SIZE);
    if (buffer == NULL)
    {
        fprintf(stderr, "Error: Out of memory\n");
        return NULL;
    }
    buffer[0] = '\0';

    for (int i = 0; i < count; i++)
    {
        HANDLE processHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processIds[i]);
        if (processHandle != NULL)
        {
            char processName[MAX_PATH];
            if (GetModuleFileNameExA(processHandle, NULL, processName, MAX_PATH))
            {
                char processIdString[16];
                snprintf(processIdString, sizeof(processIdString), "%lu", processIds[i]);
                strncat(buffer, processIdString, BUF_SIZE - strlen(buffer) - 1);
                strncat(buffer, ":", BUF_SIZE - strlen(buffer) - 1);
                strncat(buffer, processName, BUF_SIZE - strlen(buffer) - 1);
                strncat(buffer, "\n", BUF_SIZE - strlen(buffer) - 1);
            }
            CloseHandle(processHandle);
        }
    }

    return buffer;
}
