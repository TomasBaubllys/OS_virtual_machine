#include "../include/cpu.h"

int init_cpu(CPU* cpu, Memory* memory, Channel_device* channel_device) {
	if(!cpu || !channel_device || !memory) {
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
	cpu -> channel_device = channel_device;
	cpu -> memory = memory;
	return 0;
}

int interupt(CPU* cpu) {
	if(!cpu) {
		return 0;
	}

	if(cpu -> ti == 0) {
		check(cpu);
	}

	if(cpu -> pi > 0) {
		if((cpu -> sf & 0x0010) >> 4 == 1) {
			switch (cpu -> pi)
			{
			case CPU_PI_INVALID_ADDRESS:
				printf(CPU_MSG_INVALID_ADDRESS);
				break;
			case CPU_PI_DIVISION_BY_ZERO:
				printf(CPU_MSG_DIVISION_BY_ZERO);
				break;
			case CPU_PI_INVALID_ASSIGNMENT:
				printf(CPU_MSG_INVALID_ASSIGNMENT);
				break;
			case CPU_PI_INVALID_OPCODE:
				printf(CPU_MSG_INVALID_OPCODE);
				break;
			case CPU_PI_OVERFLOW:
				printf(CPU_MSG_OVERFLOW);
				break;
			default:
				break;
			}
		}
		// check if logging is on
		// if so log the err
		cpu -> pi = 0;
		cpu -> si = 0;
		return INTERUPT_STOP;

	}

	if(cpu -> si > 0) {
		cpu -> mr = CPU_SUPERVISOR_MODE;

		switch(cpu -> si){
			case CPU_SI_GEDA: {
				cpu -> channel_device -> dt = RA_REG; 
				cpu -> channel_device -> st = IO_STREAM;
				xchg(cpu -> channel_device);
				cpu -> ra = cpu -> channel_device -> sa;
				break;
			}
			case CPU_SI_PUTA: {
				cpu -> channel_device -> dt = IO_STREAM; 
				cpu -> channel_device -> st = RA_REG;
				cpu -> channel_device -> sa = cpu -> ra;
				xchg(cpu -> channel_device);
				break;
			}
			case CPU_SI_PSTR: {
				if(cpu -> rc == 0) {
					break;
				}

				// if not get the page count and loop through all of it
				do {

					// check if it all fits inside a single page
					uint16_t r_addr = translate_to_real_address(cpu -> memory, cpu -> ra);
					uint16_t r_page = (r_addr / MEM_WORD_SIZE) / MEM_PAGE_SIZE;
					uint16_t offset = r_addr - (r_page * MEM_WORD_SIZE * MEM_PAGE_SIZE);  
					uint16_t byte_count = MEM_PAGE_SIZE * MEM_WORD_SIZE - offset;
				
					if(byte_count > cpu -> rc) {
						byte_count = cpu -> rc;
					}


					// set the ch_dev register
					cpu -> channel_device -> st = USER_MEM;
					cpu -> channel_device -> dt = IO_STREAM;
					cpu -> channel_device -> of = offset;
					cpu -> channel_device -> sb = r_page;
					cpu -> channel_device -> cb = byte_count;

					// call xchg
					xchg(cpu -> channel_device);
					
					// increase the virtual address by the amount of bytes printed
					cpu -> ra += byte_count;

					// retranslate a new address
					cpu -> rc -= byte_count;
	
				} while(cpu -> rc != 0);
			
				break;
			}
			case CPU_SI_LW: {
				uint16_t r_addr = translate_to_real_address(cpu -> memory, (cpu -> ra) >> 16);
				uint16_t r_page = (r_addr / MEM_WORD_SIZE) / MEM_PAGE_SIZE;
				cpu -> channel_device -> of = r_addr - r_page * MEM_PAGE_SIZE * MEM_WORD_SIZE;
				cpu -> channel_device -> sb = r_page;
				cpu -> channel_device -> st = USER_MEM;
				cpu -> channel_device -> dt = RA_REG;

				xchg(cpu -> channel_device);

				cpu -> ra = cpu -> channel_device -> sa;
				break;
			}
			case CPU_SI_SW: {
				uint16_t r_addr = translate_to_real_address(cpu -> memory, (cpu -> rb) >> 16);
				uint16_t r_page = (r_addr / MEM_WORD_SIZE) / MEM_PAGE_SIZE;
				cpu -> channel_device -> of = r_addr - r_page * MEM_PAGE_SIZE * MEM_WORD_SIZE;
				cpu -> channel_device -> db = r_page;
				cpu -> channel_device -> dt = USER_MEM;
				cpu -> channel_device -> st = RA_REG;
				cpu -> channel_device -> sa = cpu -> ra;

				xchg(cpu -> channel_device);
				// reset the upper bytes of rb
				cpu -> rb &= 0x0000ffff;
				break;
			}
			case CPU_SI_BP: {
				// check if doesnt  exceeed max shared memory address
				if(cpu -> rb >> 16 > MEM_MAX_SHARED_ADDRESS) {
					cpu -> pi = CPU_PI_INVALID_ADDRESS;
					break;
				}

				cpu -> channel_device -> sa = cpu -> ra;
				cpu -> channel_device -> of = cpu -> rb >> 16;
				cpu -> channel_device -> dt = SHARED_MEM;
				cpu -> channel_device -> st = RA_REG;

				xchg(cpu -> channel_device);

				cpu -> rb &= 0x0000ffff;
				cpu -> ss = SEMAFOR_FREE;
				break;
			}
			case CPU_SI_BG: {
				if(cpu -> ra >> 16 > MEM_MAX_SHARED_ADDRESS) {
					cpu -> pi = CPU_PI_INVALID_ADDRESS;
					break;
				}

				cpu -> channel_device -> of = cpu -> ra >> 16;
				cpu -> channel_device -> st = SHARED_MEM;
				cpu -> channel_device -> dt = RA_REG;

				xchg(cpu -> channel_device);
				cpu -> ra = cpu -> channel_device -> sa;

				cpu -> ss = SEMAFOR_FREE;
				break;
			}
			case CPU_SI_STOP:
				cpu -> si = 0;
				cpu -> pi = 0;
				return INTERUPT_STOP;
			default:
				break;
		}

		cpu -> mr = CPU_USER_MODE;
		cpu -> ti -= 3;
	}

	cpu -> si = 0;
	cpu -> pi = 0;
	
	return 0;
}

uint8_t check(CPU* cpu) {
	if(!cpu) {
		return 0;
	}

	cpu -> ti = CPU_DEFAULT_TIMER_VALUE;
	return 0;
}

int reset_cpu(CPU* cpu) {
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