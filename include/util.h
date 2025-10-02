#ifndef UTIL_H_INCLUDED
#define UTIL_H_INCLUDED

#include <stdint.h>
#include <stdio.h>

#define UTIL_BAD_ARG 0xff

// converts characters representing hexadecimal values to decimal values
uint8_t char_hex_to_decimal(uint8_t ch);

// converts to lowercase
uint8_t letter_to_lower(uint8_t ch);

// not used anmore
void reverse_endianness_4(uint8_t* data, uint32_t length);

// not used anymore
uint32_t reverse32(uint32_t value);

// not used anymore
// reverse endianness of an array of uint32_t
void reverse_endianness_array(uint32_t *arr, uint32_t length);

// converts to characters to lower case
uint16_t uint16_t_to_lower(uint16_t reg_char);

// huh
void print_uint32_as_str(uint32_t value);

// used to print file names
void print_file_name(char* fname, uint16_t size);

#endif // UTIL_H_INCLUDED
