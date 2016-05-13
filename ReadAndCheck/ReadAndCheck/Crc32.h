//
// Created by shuai on 16/4/14.
//

#ifndef INC_7ZCRYPTOGRAPHIC_CRC32_H
#define INC_7ZCRYPTOGRAPHIC_CRC32_H

#include <stdio.h>
#include <memory.h>
//#include "memory.h"
//#include "crc32.h"
//#include "memdbg.h"

typedef unsigned int CRC32_t;
#define POLY 0xEDB88320
#define ALL1 0xFFFFFFFF

static CRC32_t table[256];
static int bInit=0;
static void CRC32_Init(CRC32_t *value)
{
    unsigned int index, bit;
    CRC32_t entry;

    *value = ALL1;
//    value = ALL1;

    if (bInit) return;
    bInit = 1;
    for (index = 0; index < 0x100; index++) {
        entry = index;

        for (bit = 0; bit < 8; bit++)
            if (entry & 1) {
                entry >>= 1;
                entry ^= POLY;
            } else
                entry >>= 1;

        table[index] = entry;
    }
}

static void CRC32_Update(CRC32_t *value, void *data, unsigned int size)
{
    unsigned char *ptr;
    unsigned int count;
    CRC32_t result;

    result = *value;
//    result = value;
    ptr = (unsigned char *)data;
    count = size;

    if (count)
        do {
            result = (result >> 8) ^ table[(result ^ *ptr++) & 0xFF];
        } while (--count);

    *value = result;
}

static void CRC32_Final(unsigned char *out, CRC32_t value)
{
    value = ~value;
    out[0] = value;
    out[1] = value >> 8;
    out[2] = value >> 16;
    out[3] = value >> 24;
}

#endif //INC_7ZCRYPTOGRAPHIC_CRC32_H
