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
