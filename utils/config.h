#define BUF_SIZE 4096

#define HOST "127.0.0.1"
#define PORT 7777

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "./types.h"
#include "./command.h"
#include "./user_io.h"
#include "./socket.h"

//=====================================================================================================================

// Strip strings
void strip(char *str)
{
    // Remove leading whitespace
    while (isspace(*str))
    {
        str++;
    }

    // Remove trailing whitespace
    char *end = str + strlen(str) - 1;
    while (end > str && isspace(*end))
    {
        *end = '\0';
        end--;
    }
}
