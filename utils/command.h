#include <string.h>
#include <windef.h>

#include "types.h"

#define NUM_COMMANDS 5
#define NUM_SPEC_CMD 2
#define INVALID_COMMAND 0xFF
#define INVALID_COMMAND_TYPE "INVALID"
#define QUIT "QUIT"

#define SHIFT 4

#define PREAMBLE_FMT "%x;;%i"
#define SPECIAL_FMT PREAMBLE_FMT ";;%s"

static char *commands[NUM_COMMANDS] = {"UPLOAD", "DOWNLOAD", "INFORMATION", "PROCESSES", QUIT};
static char *specialCommands[NUM_SPEC_CMD] = {"UPLOAD", "DOWNLOAD"};

/*
I want to have this redefined into a structure. This is a cheap, easy, and inefficient way of implementing this

typedef extern struct {
    char *commandName;
    unsigned char commandCode;
    BOOLEAN special;
} COMMAND;
*/

COMMAND encodeCommand(char *command)
{
    if (command == NULL)
    {
        return INVALID_COMMAND;
    }
    strupr(command);
    for (int i = 0; i < NUM_COMMANDS; i++)
    {
        if (strcmp(command, commands[i]) == 0)
        {
            return (0xF0 ^ (i << SHIFT)) | i;
        }
    }
    return INVALID_COMMAND;
}

BOOLEAN isSpecial(COMMAND commandCode)
{
    for (int i = 0; i < NUM_SPEC_CMD; i++)
    {
        if (commandCode == encodeCommand(specialCommands[i]))
        {
            return TRUE;
        }
    }
    return FALSE;
}

char *decodeCommand(COMMAND command)
{
    int index = 0x0F & command;
    int parity = 0x0F ^ (command >> SHIFT);
    if (index != parity)
    {
        return INVALID_COMMAND_TYPE;
    }
    else if (index >= NUM_COMMANDS)
    {
        return INVALID_COMMAND_TYPE;
    }

    return commands[index];
}
