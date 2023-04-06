#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "types.h"

UINT4 extendByte(unsigned char byte)
{
    /* A more legible version is:
    UINT4 result;
    for (int i = 0; i < sizeof(UINT4); i++)
    {
        result |= byte << (8 * i);
    }
    return result;
    */
    return byte << 24 | byte << 16 | byte << 8 | byte;
}

UINT4 generateKey(COMMAND commandCode, UINT4 seed)
{
    srand(seed);
    UINT4 random = rand(), key = 0 ^ commandCode;
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
    UINT4 key = extendByte(commandCode);
    int keyIdx = 0;
    if (rounds == 0)
    {
        printf("[!] Warning: 0 Rounds set, input will be unaltered");
    }
    for (int i = 0; i < rounds; i++)
    {
        for (int j = 0; j < bufferSize - 1; j++)
        {
            if (keyIdx % sizeof(UINT4) == 0)
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
