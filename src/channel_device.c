#include "../include/channel_device.h"

int init_channel_device(Channel_device* channel_device, Memory* memory, Hard_disk* hard_disk) {
	if(!channel_device || !memory) {
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
	channel_device -> hard_disk = hard_disk;

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
					channel_device -> sa = read_word(channel_device -> memory, channel_device -> sb * MEM_WORD_SIZE * MEM_PAGE_SIZE + channel_device -> of);
					return 0;
				case IO_STREAM:											// user memory -> stdio PSTR
					uint32_t size = (channel_device -> cb) / MEM_WORD_SIZE;
					uint8_t rem = (channel_device -> cb) % MEM_WORD_SIZE;
					uint32_t addr = channel_device -> sb * MEM_WORD_SIZE * MEM_PAGE_SIZE + channel_device -> of;

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
				case USER_MEM:									// ra -> user memory sw
					return write_word(channel_device -> memory, channel_device -> db * MEM_WORD_SIZE * MEM_PAGE_SIZE + channel_device -> of, channel_device -> sa);
				default:
					break;
			}
			break;
		case SUPER_MEM:
			switch (channel_device -> dt) {
				case USER_MEM:											// supervisor mem -> user mem (used for loading programs)
					for(uint32_t i = 0; i < channel_device -> cb; i += 4) {
						uint32_t word = read_word(channel_device -> memory, channel_device -> sb * MEM_WORD_SIZE * MEM_PAGE_SIZE + i + channel_device -> sa);
						write_word(channel_device -> memory, channel_device -> db * MEM_WORD_SIZE * MEM_PAGE_SIZE + channel_device -> of + i, word); 
					}
					break;
			
				default:
					break;
			}
			break;	
		case HD_DISK:													// used for copying file contents from hd to supermem
			switch (channel_device -> dt) {
				case SUPER_MEM:
					for(uint32_t i = 0; i < channel_device -> cb; i += 4) {
						uint32_t word = read_word_hard_disk(channel_device -> hard_disk, channel_device -> sb * MEM_PAGE_SIZE * MEM_WORD_SIZE + channel_device -> of + i);
						write_word(channel_device -> memory, channel_device -> db * MEM_PAGE_SIZE * MEM_WORD_SIZE + i, word);
					}
					break;
				
				default:
					break;
			}
			break;
		default:
			break;
	}

	return -1;
}
