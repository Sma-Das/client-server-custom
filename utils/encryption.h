#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "types.h"

INT4 generateKey(COMMAND commandCode, INT4 seed)
{
    srand(seed);
    INT4 random = rand(), key = 0 ^ commandCode;
    if (sizeof(COMMAND) != 1)
    {
        exit(90);
    }
    for (int i = 0; i < 4; i++)
    {
        random ^= key;
        key <<= 8;
    }
    return random;
}

void encrypt(COMMAND commandCode, char *buffer, int bufferSize, int rounds)
{
    INT4 key = 0x0;
    int keyIdx = 0;
    if (rounds == 0)
    {
        printf("[!] Warning: 0 Rounds set, input will be unaltered");
    }
    for (int i = 0; i < rounds; i++)
    {
        for (int j = 0; j < bufferSize - 1; j++)
        {
            if (keyIdx % sizeof(INT4) == 0)
            {
                key = generateKey(commandCode, key);
            }
            buffer[j] ^= (key >> keyIdx++ % 8) & 0xFF;
        }
    }
}

void decrypt(COMMAND commandCode, char *buffer, int bufferSize, int rounds)
{
    encrypt(commandCode, buffer, bufferSize, rounds);
}
