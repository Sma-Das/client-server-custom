#include <string.h>
#include <ctype.h>

#define NUM_COMMANDS 5
#define INVALID_COMMAND 0xFF
#define INVALID_COMMAND_TYPE "INVALID"
#define SHIFT 4
#define PREAMBLE_FMT "%s;;"

static char *commands[NUM_COMMANDS] = {"UPLOAD", "DOWNLOAD", "INFORMATION", "PROCESSES", "QUIT"};

unsigned char encodeCommand(char *command)
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

char *decodeCommand(unsigned char command)
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
