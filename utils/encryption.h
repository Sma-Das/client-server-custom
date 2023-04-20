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

#define BITS 2
#define RR 0 * BITS
#define RL 1 * BITS
#define LR 2 * BITS
#define LL 3 * BITS
#define MASK 0b11
#define L_MASK 0xF0
#define R_MASK 0x0F
#define XORBYTE 0xCAFEDEAD

void splice(char *buffer, int bufferSize)
{
    for (int i = 0; i < bufferSize; i++)
    {
        char temp = 0;
        temp |= ((buffer[i] & (MASK << LR)) >> LR) << LL;
        temp |= ((buffer[i] & (MASK << RR)) >> RR) << LR;
        temp |= ((buffer[i] & (MASK << LL)) >> LL) << RL;
        temp |= ((buffer[i] & (MASK << RL)) >> RL) << RR;
        temp ^= (0xCAFEDEAD >> (8 * (i % 4)));
        temp = (temp & R_MASK) << 4 | (temp & L_MASK) >> 4;
        buffer[i] = temp;
    }
}

void unsplice(char *buffer, int bufferSize)
{
    for (int i = 0; i < bufferSize; i++)
    {
        buffer[i] = (buffer[i] & R_MASK) << 4 | (buffer[i] & L_MASK) >> 4;
        buffer[i] ^= (0xCAFEDEAD >> (8 * (i % 4)));
        char temp = 0;
        temp |= ((buffer[i] & (MASK << RL)) >> RL) << LL;
        temp |= ((buffer[i] & (MASK << LL)) >> LL) << LR;
        temp |= ((buffer[i] & (MASK << RR)) >> RR) << RL;
        temp |= ((buffer[i] & (MASK << LR)) >> LR) << RR;
        buffer[i] = temp;
    }
}

#undef RR_MASK
#undef RL_MASK
#undef LR_MASK
#undef LL_MASK
#undef L_MASK
#undef R_MASK
#undef XORBYTE

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
