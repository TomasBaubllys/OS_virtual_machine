#include "../include/cpu.h"

int init_cpu(CPU* cpu) {
	if(!cpu) {
		return -1;
	}
	
	cpu -> pc = 0;
	cpu -> pi = 0;
	cpu -> si = 0;
	cpu -> tr = 0;
	cpu -> ti = CPU_DEFAULT_TIMER_VALUE;
	cpu -> sf = 0;
	cpu -> mr = CPU_USER_MODE;
	cpu -> ss = 0;
	cpu -> ra = 0;
	cpu -> rb = 0;
	cpu -> rc = 0;
	
	return 0;
}

uint8_t get_register_num(uint16_t reg_char) {
	uint8_t u_letter = letter_to_lower(reg_char >> 8);
	uint8_t l_letter = letter_to_lower(reg_char & 0x00ff);
	
	if(u_letter == UTIL_BAD_ARG || l_letter == UTIL_BAD_ARG) {
		return CPU_UNKNOWN_REGISTER;
	}

	reg_char = (u_letter << 8) | l_letter;

	switch(reg_char) {
		case 0x7063:
			return PC;
		case 0x7069:
			return PI;
		case 0x7369:
			return SI;
		case 0x7472:
			return TR;
		case 0x7469:
			return TI;
		case 0x7366:
			return SF;
		case 0x6d72:
			return MR;
		case 0x7373:
			return SS;
		case 0x7261:
			return RA;
		case 0x7262:
			return RB;
		case 0x7263:
			return RC; 
		default:
			break;
	}

	return CPU_UNKNOWN_REGISTER;
}
