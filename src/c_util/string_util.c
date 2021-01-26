#include "string_util.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

char* uint8_t_to_bin(uint8_t value)
{
    static char buf[8+1];
    memset(buf, 8, '0');
    for (int8_t x = 7; x >= 0; --x)
    {
        if (value & 0x01)
        {
            buf[x] = '1';
        }
        else
        {
            buf[x] = '0';
        }
        (value >>= (uint8_t)1);
    }
    buf[8] = '\0';
    return buf;
}

char* uint8_t_to_dec(uint8_t value)
{
    static char buf[3+1];
    int len = snprintf(buf, 3+1, "%u", value);
    if (len >= 3+1)
    {
        abort();
    }
    return buf;
}
char* uint8_t_to_hex(uint8_t value)
{
    static char buf[4+1];
    int len = snprintf(buf, 3+1, "%X", value);
    if (len >= 4+1)
    {
        abort();
    }
    return buf;
}

char* uint16_t_to_bin(uint8_t value)
{
    static char buf[16+1];
    memset(buf, 16, '0');
    for (int8_t x = 15; x >= 0; --x)
    {
        if (value & 0x01)
        {
            buf[x] = '1';
        }
        else
        {
            buf[x] = '0';
        }
        (value >>= (uint16_t)1);
    }
    buf[16] = '\0';
    return buf;
}

char* uint16_t_to_dec(uint8_t value)
{
    static char buf[6+1];
    int len = snprintf(buf, 6+1, "%u", value);
    if (len >= 6+1)
    {
        abort();
    }
    return buf;
}
char* uint16_t_to_hex(uint8_t value)
{
    static char buf[6+1];
    int len = snprintf(buf, 6+1, "%X", value);
    if (len >= 6+1)
    {
        abort();
    }
    return buf;
}