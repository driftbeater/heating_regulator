#ifndef STRING_UTIL_H_
#define STRING_UTIL_H_

#include <avr/io.h>

char* uint8_t_to_bin(uint8_t value);
char* uint8_t_to_dec(uint8_t value);
char* uint8_t_to_hex(uint8_t value);
char* uint16_t_to_bin(uint8_t value);
char* uint16_t_to_dec(uint8_t value);
char* uint16_t_to_hex(uint8_t value);

#endif