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

void print_uint32_as_str(uint32_t value) {
    char str[5];  // 4 chars + null terminator
    str[0] = (value >> 24) & 0xFF;  // most significant byte
    str[1] = (value >> 16) & 0xFF;
    str[2] = (value >> 8)  & 0xFF;
    str[3] = value & 0xFF;          // least significant byte
    str[4] = '\0';                  // null terminator

    printf("%s\n", str);
}

// proccesed in bytes of 4
void print_file_name(char* fname, uint16_t size) {
	if(size % 4 != 0 || size == 0) {
		return;
	}

	for(uint16_t i = 0; i < size; i += 4) {
		putchar(fname[i + 3]);
		putchar(fname[i + 2]);
		putchar(fname[i + 1]);
		putchar(fname[i]);
	}
}