#include <string.h>
#include <windef.h>
#include <stdio.h>

#include "types.h"

#define NUM_COMMANDS 5
#define NUM_SPEC_CMD 2
#define INVALID_COMMAND 0xFF
#define INVALID_COMMAND_TYPE "INVALID"
#define QUIT "QUIT"

#define SHIFT 4

#define PREAMBLE_FMT "%hhu;;%i"
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

/**
 * Encode a command based on its index where the high bits is the negation of the index
 * and the low bits are the index
 *
 * @param command Command from the command array: return INVALID otherwise
 *
 * @return one byte representation of the command
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

/**
 * Check if a given commandCode is a special command
 *
 * @param commandCode single byte representation
 *
 * @return BOOL
 */
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

/**
 * Decode a given command code and verify that it was not currupted
 *
 * @param command representation of the command
 *
 * @return Command Name of the corresponding command else INVALID_COMMAND
 */
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

/**
 * Parse a given buffer into its corresponding parts accounting for special fields
 *
 * @param buffer buffer to read from
 * @param commandCode buffer to write into
 * @param bufferSize buffer to write into
 * @param URL buffer to write into
 * @param special is it a special command
 *
 */
void parseCommand(char *buffer, COMMAND *commandCode, int *bufferSize, char *URL, BOOLEAN special)
{
    if (special)
    {
        sscanf(buffer, SPECIAL_FMT, commandCode, bufferSize, URL);
    }
    else
    {
        sscanf(buffer, PREAMBLE_FMT, commandCode, bufferSize);
    }
}
