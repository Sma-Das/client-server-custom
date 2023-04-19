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
    encrypt(command, buffer, bufferSize, 0x800);
    printf("%s\n", buffer);
<<<<<<< Updated upstream
=======
    splice(buffer, bufferSize);
    // printf("%s\n", buffer);
    for (int i = 0; i < bufferSize; i++) {
       buffer[i] ^= 0xCAFEDEAD >> (8 * (i % 4)); 
    }
    // printf("%s\n", buffer);
>>>>>>> Stashed changes
    return 0;
}
