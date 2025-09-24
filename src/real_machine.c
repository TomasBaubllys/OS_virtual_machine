#include "../include/real_machine.h"

int init_real_machine(Real_machine* real_machine) {
	if(!real_machine) {
		return -1;
	}

	if(init_cpu(&(real_machine -> cpu)) != 0) {
		return -1;
	}

	if(init_memory(&(real_machine -> mem)) != 0) {
		return -1;
	}

	if(init_hard_disk(&(real_machine -> hd)) != 0) {
		return -1;
	}

	if(init_channel_device(&(real_machine -> ch_dev)) != 0) {
		return -1;
	}

	return 0;
};

int execute_command(Real_machine* real_machine, uint32_t command) {
	uint16_t com_code = command >> 16;
	uint16_t args = command & 0x0000ffff;				// gets trown away in some cases	

	switch(com_code) {
		// memory commands
		// MOxy
		case 0x4d4f: {
			uint8_t x = char_hex_to_decimal((command & 0x0000ff00) >> 8);
			uint8_t y = char_hex_to_decimal(command & 0x000000ff);
			if(x == 0xff || y == 0xff) {
				return -1;
			}
			
			real_machine -> cpu.ra = x * 16 + y;
			break;
		}
		// LRa 
		// copy RA value to a value
		case 0x4c52: {	
			uint16_t reg = command & 0x0000ffff;
			uint8_t reg_nr = get_register_num(reg);
			if(reg_nr == CPU_UNKNOWN_REGISTER) {
				return -1;
			}

			switch (reg_nr){
				case RA:
					// nothing
					break;
				case RB:
					real_machine -> cpu.rb = real_machine -> cpu.ra;
					break;
				case RC:
					real_machine -> cpu.rc = real_machine -> cpu.ra;
					break;
				default:
					// other registers not allowed
					return -1;
			}			
				
			break;
		}
		// LLa
		// copy a value to RB value
		case 0x4c4c:{
			uint16_t reg = command & 0x0000ffff;
			uint8_t reg_nr = get_register_num(reg);
			if(reg_nr == CPU_UNKNOWN_REGISTER){
				return -1;
			}

			switch (reg_nr){
				case RA:
					real_machine -> cpu.rb = real_machine -> cpu.ra;
					break;
				case RB:
					// nothing
					break;
				case RC:
					real_machine -> cpu.rb = real_machine -> cpu.rc;
					break;
				default:
					return -1;
			}
		}
		// LBxy
		// value at address x*16 + y copying to RA
		// 
	

		// LR 

		// aritmetic commands
		// APxy	
		case 0x4150: {
			uint8_t x = char_hex_to_decimal((command & 0x0000ff00) >> 8);
			uint8_t y = char_hex_to_decimal(command & 0x000000ff);
			if(x == 0xff || y == 0xff) {
				return -1;
			}
			
			real_machine -> cpu.ra = x * 16 + y;
			break;
		}
		// ADa
		// add a value to RA 
		case 0x4144:{
			uint16_t reg = command & 0x0000ffff;
			uint8_t reg_nr = get_register_num(reg);
			if(reg_nr == CPU_UNKNOWN_REGISTER) {
				return -1;
			}

			switch (reg_nr){
				case RA:
					real_machine -> cpu.ra += real_machine -> cpu.ra; 
					break;
				case RB:
					real_machine -> cpu.ra += real_machine -> cpu.rb;
					break;
				case RC:
					real_machine -> cpu.ra += real_machine -> cpu.rc;
					break;
				default:
					// other registers not allowed
					return -1;
			}			
				
			break;
		}
		// SBa
		// substract a value to RA 
		case 0x5342:{
			uint16_t reg = command & 0x0000ffff;
			uint8_t reg_nr = get_register_num(reg);
			if(reg_nr == CPU_UNKNOWN_REGISTER) {
				return -1;
			}

			switch (reg_nr){
				case RA:
					real_machine -> cpu.ra -= real_machine -> cpu.ra; 
					break;
				case RB:
					real_machine -> cpu.ra -= real_machine -> cpu.rb;
					break;
				case RC:
					real_machine -> cpu.ra -= real_machine -> cpu.rc;
					break;
				default:
					// other registers not allowed
					return -1;
			}			
				
			break;
		}
		// MUa
		// multiplies a value by RA value 
		// carry flag
		// sf 0000011
		case 0x4D55:{
			uint16_t reg = command & 0x0000ffff;
			uint8_t reg_nr = get_register_num(reg);
			if(reg_nr == CPU_UNKNOWN_REGISTER) {
				return -1;
			}

			switch (reg_nr){
				case RA:
					real_machine -> cpu.ra *= real_machine -> cpu.ra; 
					break;
				case RB:
					real_machine -> cpu.ra *= real_machine -> cpu.rb;
					break;
				case RC:
					real_machine -> cpu.ra *= real_machine -> cpu.rc;
					break;
				default:
					// other registers not allowed
					return -1;
			}			
				
			break;
		}
		// DVa
		// dividexs RA value by a value 
		case 0x4456:{
			uint16_t reg = command & 0x0000ffff;
			uint8_t reg_nr = get_register_num(reg);
			if(reg_nr == CPU_UNKNOWN_REGISTER) {
				return -1;
			}

			switch (reg_nr){
				case RA:
					if(real_machine -> cpu.ra == 0){
						real_machine -> cpu.si = 5;
						return 0;
					}
					real_machine -> cpu.ra /= real_machine -> cpu.ra; 
					
					break;
				case RB:
					if(real_machine -> cpu.rb == 0){
						real_machine -> cpu.si = 5;
						return 0;
					}
					uint32_t temp = real_machine -> cpu.ra;
					real_machine -> cpu.ra /= real_machine -> cpu.rb;
					real_machine -> cpu.rb  = temp % real_machine -> cpu.rb;
					break;
				case RC:
					if(real_machine -> cpu.rc == 0){
						real_machine -> cpu.si = 5;
						return 0;
					}
					uint32_t temp1 = real_machine -> cpu.ra;
					real_machine -> cpu.ra /= real_machine -> cpu.rc;
					real_machine -> cpu.rc  = temp1 % real_machine -> cpu.rc;
					break;
				default:
					// other registers not allowed
					return -1;
			}			
				
			break;
		}
		// CMcy
		// (x*16 + y == ra)
		case 0x434d:{
			uint8_t x = char_hex_to_decimal((command & 0x0000ff00) >> 8);
			uint8_t y = char_hex_to_decimal(command & 0x000000ff);
			if(x == 0xff || y == 0xff) {
				return -1;
			}

			// ZF = 1 CF = 0
			if(x * 16 + y == real_machine -> cpu.ra){
				real_machine -> cpu.sf ^= 0x0002; // 0000 0000 0000 0010
				real_machine -> cpu.sf &= 0xfffe; // 1111 1111 1111 1110
			}
			// ZF = 0 CF = 1
			else if(x * 16 + y < real_machine -> cpu.ra){
				real_machine -> cpu.sf &=  0xfffd; // 1111 1111 1111 1101
				real_machine -> cpu.sf ^= 0x0001; // 0000 0000 0000 0001
			}
			// ZF = 0 CF = 0
			else{
				real_machine -> cpu.sf &= 0xfffd;
				real_machine -> cpu.sf &= 0xfffe;
			}

			break;
		}
		// CRRB
		// rb == ra
		case 0x4352:{
			uint16_t reg = command & 0x0000ffff;
			uint8_t reg_nr = get_register_num(reg);

			if(reg_nr != RB){
				return -1;
			}


			// ZF = 1 CF = 0
			if(real_machine -> cpu.rb == real_machine -> cpu.ra){
				real_machine -> cpu.sf ^= 0x0002; // 0000 0000 0000 0010
				real_machine -> cpu.sf &= 0xfffe; // 1111 1111 1111 1110
			}
			// ZF = 0 CF = 1
			else if(real_machine -> cpu.rb < real_machine -> cpu.ra){
				real_machine -> cpu.sf &= 0xfffd; // 1111 1111 1111 1101
				real_machine -> cpu.sf ^= 0x0001; // 0000 0000 0000 0001
			}
			// ZF = 0 CF = 0
			else{
				real_machine -> cpu.sf &= 0xfffd;
				real_machine -> cpu.sf &= 0xfffe;
			}

			break;
		}
		// cycle commands 
		// LOxy
		case 0x4c4f: {
			uint8_t x = char_hex_to_decimal((command & 0x0000ff00) >> 8);
			uint8_t y = char_hex_to_decimal(command & 0x000000ff);
			if(real_machine -> cpu.rc > 0) {
				if(x == 0xff || y == 0xff) {
					return -1;
				}
				
				--(real_machine -> cpu.rc);
				(real_machine -> cpu.pc) = x * 16 + y; 
			}
			break;
		}
		// logic operations
		// XRa
		// a XOR RA
		case 0x5852:{
			uint16_t reg = command & 0x0000ffff;
			uint8_t reg_nr = get_register_num(reg);

			if(reg == CPU_UNKNOWN_REGISTER){
				return -1;
			}

			switch (reg_nr){
				case RA:
					real_machine -> cpu.ra ^= real_machine -> cpu.ra; 
					break;
				case RB:
					real_machine -> cpu.ra ^= real_machine -> cpu.rb;
					break;
				case RC:
					real_machine -> cpu.ra ^= real_machine -> cpu.rc;
					break;
				default:
					return -1;
					break;
			}

			break;
		}
		// ANa
		// a AND RA
		case 0x414e:{
			uint16_t reg = command & 0x0000ffff;
			uint8_t reg_nr = get_register_num(reg);
			
			if(reg == CPU_UNKNOWN_REGISTER){
				return -1;
			}

			switch (reg_nr)
			{
				case RA:
					real_machine -> cpu.ra &= real_machine -> cpu.ra;
					break;
				case RB:
					real_machine -> cpu.ra &= real_machine -> cpu.rb;
					break;
				case RC:
					real_machine -> cpu.ra &= real_machine -> cpu.rb;
					break;
				default:
					return -1;
					break;
			}

			break;
		}
		// NOa
		// NOT a
		case 0x4e4f:{
			uint16_t reg = command & 0x0000ffff;
			uint8_t reg_nr = get_register_num(reg);

			if(reg == CPU_UNKNOWN_REGISTER){
				return -1;
			}

			switch (reg_nr)
			{
				case RA:
					real_machine -> cpu.ra = ~real_machine -> cpu.ra;
					break;
				case RB:
					real_machine -> cpu.rb = ~real_machine -> cpu.rb;
					break;
				case RC:
					real_machine -> cpu.rc = ~real_machine -> cpu.rc;
					break;
				default:
					return -1;
					break;
			}

			break;

		}

		// JUMPS
		// JUxy
		case 0x4a55: {
			uint8_t x = char_hex_to_decimal((command & 0x0000ff00) >> 8);
			uint8_t y = char_hex_to_decimal(command & 0x000000ff);
			if(x == 0xff || y == 0xff) {
				return -1;
			}
			
			real_machine -> cpu.pc = x * 16 + y;
			break;
		}
		// JZxy
		// control to address x*16+y  if ZF=1
		case 0x4a5a:{
			uint8_t x = char_hex_to_decimal((command & 0x0000ff00) >> 8);
			uint8_t y = char_hex_to_decimal(command & 0x000000ff);
			if(x == 0xff || y == 0xff) {
				return -1;
			}

			if(((real_machine -> cpu.sf & 0x0002) >> 1) == 1){
				real_machine -> cpu.pc = x * 16 + y;
			}

		}


		default:
			return -1;
	}
	
	return 0;
}
