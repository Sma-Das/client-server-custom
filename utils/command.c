#include <string.h>
#include <ctype.h>

#define NUM_COMMANDS 4
#define INVALID_COMMAND 0xFF
#define INVALID_COMMAND_TYPE "INVALID"

static char *commands[NUM_COMMANDS] = {"UPLOAD", "DOWNLOAD", "INFORMATION", "PROCESSES"};

// Converts a string inplace to uppercase
void to_uppercase(char *string)
{
    while (*string)
    {
        *string = toupper(*string);
        string++;
    };
}

unsigned char encodeCommand(char *command)
{

    to_uppercase(command);
    for (int i = 0; i < NUM_COMMANDS; i++)
    {
        if (strcmp(command, commands[i]) == 0)
        {
            return (0xF0 ^ (1 << i << 4)) | 1 << i;
        }
    }
    return INVALID_COMMAND;
}

char *decodeCommand(unsigned char command)
{
    unsigned int cmd_index = 0x0F & command;
    if ((~command & 0xf0) >> 4 != cmd_index)
    {
        return INVALID_COMMAND_TYPE;
    }
    else if (cmd_index >= 1 << NUM_COMMANDS)
    {
        return INVALID_COMMAND_TYPE;
    }
    int i = 0;
    while (cmd_index >> i++)
        ;
    return commands[i - 2];
}