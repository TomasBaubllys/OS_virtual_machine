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
;

	switch(com_code) {
		// memory commands
		// MOxy
		case 0x4d4f:
			uint8_t x = char_hex_to_decimal((command & 0x0000ff00) >> 8);
			uint8_t y = char_hex_to_decimal(command & 0x000000ff);
			real_machine -> cpu.ra = x * 16 + y;
			break;

		// aritmetic commands
		// APxy	
		case 0x4150:
			uint8_t x = char_hex_to_decimal((command & 0x0000ff00) >> 8);
			uint8_t y = char_hex_to_decimal(command & 0x000000ff);
			real_machine -> cpu.ra = x * 16 + y;
		
		// cycle commands
		case 0x4c4f:
			if(real_machine -> cpu.rc > 0) {
				uint8_t x = char_hex_to_decimal((command & 0x0000ff00) >> 8);
				uint8_t y = char_hex_to_decimal(command & 0x000000ff);
				--(real_machine -> cpu.rc)
				(real_machine -> cpu.pc) = x * 16 + y; 
			}
		default:
			return -1;
	}
	
	return 0;
}
