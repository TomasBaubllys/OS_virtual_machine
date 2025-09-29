#include "../include/channel_device.h"

int init_channel_device(Channel_device* channel_device, Memory* memory) {
	if(!channel_device || memory) {
		return -1;
	}
	
	channel_device -> sb = 0;
	channel_device -> db = 0;
	channel_device -> st = 0;
	channel_device -> cb = 0;
	channel_device -> of = 0;
	channel_device -> dt = 0;
	channel_device -> sa = 0;
	channel_device -> memory = memory;

	return 0;
}

void reset_channel_device(Channel_device* channel_device){
	if(!channel_device){
		return;
	}

	channel_device -> sb = 0;
	channel_device -> db = 0;
	channel_device -> st = 0;
	channel_device -> cb = 0;
	channel_device -> of = 0;
	channel_device -> dt = 0;
	channel_device -> sa = 0;

	return;
}

// page_table_index is used for pstr to translate address
int xchg(Channel_device* channel_device) {
	// "Performancas nesvarbu svarbu, svarbu, kad veiktu" - M. Grubliauskis 22/09/2025
	if(!channel_device) {
		return -1;
	}

	switch(channel_device -> st) {
		case USER_MEM:
			switch(channel_device -> dt) {
				case RA_REG:											// user memory -> ra register LW
					channel_device -> sa = read_word(channel_device -> memory, channel_device -> sb + channel_device -> of);
					return 0;
				case IO_STREAM:											// user memory -> stdio PSTR
					uint32_t size = (channel_device -> cb) / MEM_WORD_SIZE;
					uint8_t rem = (channel_device -> cb) % MEM_WORD_SIZE;
					uint32_t addr = channel_device -> sb + channel_device -> of;

					if(rem != 0) {
						++size;
					}

					uint32_t byte_count = channel_device -> cb;
					for(uint32_t i = 0; i < size; ++i) {
						uint32_t word = read_word(channel_device -> memory, addr);
						addr += MEM_WORD_SIZE;
						for(uint8_t b = 0; b < MEM_WORD_SIZE && byte_count > 0; ++b) {
							uint8_t ch = (word >> (8 * (MEM_WORD_SIZE - 1 - b))) & 0xff;
							putchar(ch);
							--byte_count;
						}
					}
					break;
				default:
					break;
			}
			break;
		case IO_STREAM:
			switch(channel_device -> dt) {
				case RA_REG:											// stdio -> ra reg GEDA
					if(scanf("%x", &(channel_device -> sa)) != 1) {
						return -1;
					}
					return 0;
				default:
					break;
			}

			break;
		case SHARED_MEM:
			switch(channel_device -> dt) {
				case RA_REG:											// shared memory -> ra BG
					channel_device -> sa = read_word(channel_device -> memory, channel_device -> of + MEM_BEG_SHARED_MEM);
					return 0;
				default:
					break;
			}
			break;
		case RA_REG:
			switch(channel_device -> dt) {
				case SHARED_MEM:										// ra -> shared memory BP
					return write_word(channel_device -> memory, channel_device -> of + MEM_BEG_SHARED_MEM, channel_device -> sa);
				case IO_STREAM:											// ra -> stdio PUTA
					printf("%x", channel_device -> sa);
					return 0;
				case USER_MEM:											// ra -> user memory sw
					return write_word(channel_device -> memory, channel_device -> db + channel_device -> of, channel_device -> sa);
				default:
					break;
			}
			break;
		default:
			break;
	}

	// BPxy xy must be between 0 - 1F and contained in OF
	/*if(real_machine -> ch_dev.dt == RA_REG && real_machine -> ch_dev.st == SHARED_MEM) {
		if(real_machine -> cpu.mr != 1) {
			return -1;
		}


		if(real_machine -> ch_dev.of >=  MEM_MAX_SHARED_ADDRESS) {
			return -1;
		}

		real_machine -> cpu.ra = read_word(&(real_machine -> mem), real_machine -> ch_dev.of + MEM_BEG_SHARED_MEM);

		real_machine -> cpu.ss = 0;
		return 0;
	}*/

	// BGxy xy must be between 0 - 1F and contained in OF
	/*if(real_machine -> ch_dev.dt == SHARED_MEM && real_machine -> ch_dev.st == RA_REG) {
		if(real_machine -> cpu.mr != 1) {
			return -1;
		}

		if(real_machine -> ch_dev.of >=  MEM_MAX_SHARED_ADDRESS) {
			return -1;
		}

		if(write_word(&(real_machine -> mem), real_machine -> ch_dev.of + MEM_BEG_SHARED_MEM, real_machine -> cpu.ra) == 0){
			real_machine -> cpu.ss = 0;
			return 0;
		}
		else{
			real_machine -> cpu.ss = 0;
			return -1;
		}

	}*/

	return -1;
}
