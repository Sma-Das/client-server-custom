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

#define RR_MASK 0b00000011
#define RL_MASK RR_MASK << 2
#define LR_MASK RL_MASK << 2
#define LL_MASK LR_MASK << 2
#define L_MASK 0xF0
#define R_MASK 0x0F
#define XORBYTE 0xCAFEDEAD

void splice(char *buffer, int bufferSize)
{
    for (int i = 0; i < bufferSize; i++)
    {
        char temp = buffer[i];
        temp = (temp & LR_MASK) | (temp & LL_MASK) | (temp & RR_MASK) | (temp & RL_MASK);
        temp ^= XORBYTE;
        temp = (temp & R_MASK) | (temp & L_MASK);
        buffer[i] = temp;
    }
}

void unsplice(char *buffer, int bufferSize)
{
    for (int i = 0; i < bufferSize; i++)
    {
        char temp = buffer[i];
        temp = (temp & R_MASK) | (temp & L_MASK);
        temp ^= XORBYTE;
        temp = (temp & LR_MASK) | (temp & LL_MASK) | (temp & RR_MASK) | (temp & RL_MASK);
        buffer[i] = temp;
    }
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
        splice(buffer, bufferSize);
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
    UINT4 key = extendByte(commandCode);
    int keyIdx = 0;
    if (rounds == 0)
    {
        printf("[!] Warning: 0 Rounds set, input will be unaltered");
    }
    for (int i = 0; i < rounds; i++)
    {
        unsplice(buffer, bufferSize);
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
