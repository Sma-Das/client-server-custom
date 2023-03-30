#include <stdio.h>

#include "./config.h"

char *getUserInput(char *prompt, int size)
{
    char buffer[size];
    printf("%s", prompt);
    return fgets(buffer, size, stdin);
}

void getInput(char *prompt, char buffer[BUF_SIZE])
{
    char *input = getUserInput(prompt, BUF_SIZE);
    strcpy(buffer, input);
}

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
