#ifndef UTIL_H_INCLUDED
#define UTIL_H_INCLUDED

#include <stdint.h>

#define UTIL_BAD_ARG 0xff

// converts characters representing hexadecimal values to decimal values
uint8_t char_hex_to_decimal(uint8_t ch);

// converts to lowercase
uint8_t letter_to_lower(uint8_t ch);

#endif // UTIL_H_INCLUDED
