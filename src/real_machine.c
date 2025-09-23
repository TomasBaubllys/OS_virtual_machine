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
		case 0x4c52: {	
			uint16_t reg = command & 0x0000ffff;
			uint8_t reg_nr = get_regiser_num(reg);
			if(reg_nr == CPU_UNKNOWN_REGISTER) {
				return -1;
			}

			// copy RA to reg
			
				
			break;
		}
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
		default:
			return -1;
	}
	
	return 0;
}
