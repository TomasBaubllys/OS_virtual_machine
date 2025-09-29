#include "../include/util.h"

uint8_t char_hex_to_decimal(uint8_t ch) {
	if(ch >= '0' && ch <= '9') {
		return ch - '0';
	}
	
	if(ch >= 'A' && ch <= 'F') {
		return 10 + (ch - 'A');
	}

	if(ch >= 'a' && ch <= 'f') {
		return 10 + (ch - 'a');
	}

	return 0xff;
}


uint8_t letter_to_lower(uint8_t ch) {
	if(ch >= 'a' && ch <= 'z') {
		return ch;
	}	

	if(ch >= 'A' && ch <= 'Z') {
		return ch - 'A' + 'a';
	}

	return UTIL_BAD_ARG;
}


uint32_t reverse32(uint32_t value) {
    return ((value >> 24) & 0x000000FF) |
           ((value >>  8) & 0x0000FF00) |
           ((value <<  8) & 0x00FF0000) |
           ((value << 24) & 0xFF000000);
}

// Reverse endianness of an array of uint32_t
void reverse_endianness_array(uint32_t *arr, uint32_t length) {
    for (uint32_t i = 0; i < length; i++) {
        arr[i] = reverse32(arr[i]);
    }
}

uint16_t uint16_t_to_lower(uint16_t reg_char) {
	uint8_t u_letter = letter_to_lower(reg_char >> 8);
	uint8_t l_letter = letter_to_lower(reg_char & 0x00ff);

	return (u_letter << 8) | l_letter;
}

