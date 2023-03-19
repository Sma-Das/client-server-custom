#include <stdio.h>

char *getUserInput(char *prompt, int size)
{
    char buffer[size];
    printf("%s", prompt);
    return fgets(buffer, size, stdin);
}
