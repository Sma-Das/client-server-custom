#include "./utils/ftp.h"
#include "./utils/config.h"
#include "./utils/encryption.h"
#include <stdio.h>
#include <string.h>

int main()
{
    char buffer[BUF_SIZE] = "Hello, There! My name is Sma Das";
    int bufferSize = strlen(buffer);
    COMMAND command = 0xE1;
    int rounds = 8;
    encrypt(command, buffer, bufferSize, rounds);
    printf("%s\n", buffer);
    decrypt(command, buffer, bufferSize, rounds);
    printf("%s\n", buffer);
    return 0;
}
