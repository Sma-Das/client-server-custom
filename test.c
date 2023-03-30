#include <stdio.h>
#include <string.h>

#include "utils/command.h"
#include "utils/encryption.h"

void main(void)
{
    char command[] = "UPLOAD";
    COMMAND commandCode = encodeCommand(command);
    char buffer[] = "Hello, World!";
    encrypt(commandCode, buffer, sizeof(buffer), 1);
    printf("%x", buffer[13]);
    encrypt(commandCode, buffer, sizeof(buffer), 1);
    printf("%s\n", buffer);
}
