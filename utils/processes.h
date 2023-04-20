#include <windows.h>
#include <psapi.h>
#include <stdio.h>
#include <string.h>

#include "config.h"

/**
 * Retrieves a list of processes running on the specified URL and stores
 * the result in the provided buffer. The buffer must have a size of at
 * least BUF_SIZE bytes. Each process in the list is represented as a
 * string containing the process ID, name, and status, separated by
 * whitespace. The list is terminated by a null character.
 *
 * @param buffer The buffer to store the list of processes.
 * @param URL The URL of the machine to query for process information.
 *
 * @return This function does not return a value.
 */

void getProcesses(char buffer[BUF_SIZE], char *URL)
{
    DWORD processIds[128];
    DWORD bytesReturned;
    if (!EnumProcesses(processIds, sizeof(processIds), &bytesReturned))
    {
        fprintf(stderr, "[E] Error: EnumProcesses failed with error %lu\n", GetLastError());
        return;
    }
    int count = bytesReturned / sizeof(DWORD);

    if (buffer == NULL)
    {
        fprintf(stderr, "[E] Error: Out of memory\n");
        return;
    }
    buffer[0] = CF_NULL;

    for (int i = 0; i < count; i++)
    {
        HANDLE processHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processIds[i]);
        if (processHandle != NULL)
        {
            // Iterate through and add each process the buffer
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
}
