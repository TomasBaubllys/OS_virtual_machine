#include "../include/real_machine.h"
#include "../include/virtual_machine.h"

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
	
	// allocate space for RM_VM_MAX_COUNT virtual machines
	real_machine -> vm = malloc(sizeof(Virtual_machine) * RM_VM_MAX_COUNT);
		
	real_machine -> vm_count = 0;
	
	return 0;
};

int add_virtual_machine(Real_machine* real_machine, Virtual_machine* virtual_machine) {
	if(!real_machine || !virtual_machine) {
		return -1;
	} 

	if(real_machine -> vm_count >= RM_VM_MAX_COUNT) {
		return -1;
	}

	real_machine -> vm[real_machine -> vm_count] = *virtual_machine;
	++(real_machine -> vm_count);
	return 0;
}

int copy_virtual_machine(Real_machine* real_machine, uint8_t virtual_machine_index) {
	if(!real_machine) {
		return -1;
	}

	if(!real_machine -> vm) {
		return -1;
	}

	if(virtual_machine_index >= real_machine -> vm_count) {
		return -1;
	}

	// cannot copy dead machines status
	if((real_machine -> vm[virtual_machine_index]).status == 0) {
		return -1;
	}

	real_machine -> cpu.ra = real_machine -> vm[virtual_machine_index].ra;
	real_machine -> cpu.rb = real_machine -> vm[virtual_machine_index].rb;
	real_machine -> cpu.rc = real_machine -> vm[virtual_machine_index].rc;
	real_machine -> cpu.sf = real_machine -> vm[virtual_machine_index].sf;

	// translate pc to its real value using addresing table
	uint16_t v_addr = real_machine -> vm[virtual_machine_index].pc;
	uint8_t pg_index = real_machine -> vm[virtual_machine_index].page_table_index;
		
	uint16_t real_address = translate_to_real_address(real_machine, v_addr, pg_index);	

	if(real_address == VM_REAL_MACHINE_NULL || real_address == VM_MAX_VIRTUAL_ADDDRESS_EXCEEDED) {
		return -1;
	}
	
	// swap out the page to the real page
	real_machine -> cpu.pc = real_address;

	return 0;
}

int write_virtual_machine(Real_machine* real_machine, uint8_t virtual_machine_index) {
	if(!real_machine) {
		return -1;
	}

	if(!real_machine -> vm) {
		return -1;
	}

	if(virtual_machine_index >= real_machine -> vm_count) {
		return -1;
	}

	// cannot write to a killed machine
	if((real_machine -> vm[virtual_machine_index]).status == 0) {
		return -1;
	}

	real_machine -> vm[virtual_machine_index].ra = real_machine -> cpu.ra;
	real_machine -> vm[virtual_machine_index].rb = real_machine -> cpu.rb;
	real_machine -> vm[virtual_machine_index].rc = real_machine -> cpu.rc;
	real_machine -> vm[virtual_machine_index].sf = real_machine -> cpu.sf;

	// increment virtual machines pc by word size since command was completed
	// real_machine -> vm[virtual_machine_index].pc += MEM_WORD_SIZE;
	// translate the address back to virtual memory
	
	return 0;
}



// not implemented yet
int destroy_real_machine(Real_machine* real_machine) {
	if(!real_machine) {
		return -1;
	}	
	
	close_hard_disk(&(real_machine -> hd));
	free(real_machine -> vm);
	real_machine -> vm = NULL;

	return 0;
}

int remove_virtual_machine(Real_machine* real_machine, uint8_t virtual_machine_index) {
	if(!real_machine) {
		return -1;
	}

	if(virtual_machine_index >= real_machine -> vm_count) {
		return -1;
	}

	if(destroy_virtual_machine(real_machine, &(real_machine -> vm[virtual_machine_index])) != 0) {
		return -1;
	}
	
	// move reposition other machines in its place
	for(uint8_t i = virtual_machine_index; i < real_machine -> vm_count - 1; ++i) {
		real_machine -> vm[i] = real_machine -> vm[i + 1];
	}

	--(real_machine -> vm_count);

	return 0;
}

// page_table_index is used for pstr to translate address
int xchg(Real_machine* real_machine, uint8_t page_table_index) {
	// "Performancas nesvarbu svarbu, svarbu, kad veiktu" - M. Grubliauskis 22/09/2025
	if(!real_machine) {
		return -1;
	}
	
	// used by LWxy command of register must contain the address 
	if(real_machine -> ch_dev.dt == RA_REG && real_machine -> ch_dev.st == USER_MEM) {
		real_machine -> cpu.ra = read_word(&(real_machine -> mem), real_machine -> ch_dev.of);	
		return 0;
	}
		
	// used by SWxy command of register must contain the address
	if(real_machine -> ch_dev.dt == USER_MEM && real_machine -> ch_dev.st == RA_REG) {
		return write_word(&(real_machine -> mem), real_machine -> ch_dev.of, real_machine -> cpu.ra); 
	}

	// BPxy xy must be between 0 - 1F and contained in OF
	if(real_machine -> ch_dev.dt == RA_REG && real_machine -> ch_dev.st == SHARED_MEM) {
		if(real_machine -> cpu.mr != 1) {
			return -1;
		}		
		
		
		if(real_machine -> ch_dev.of >=  MEM_MAX_SHARED_ADDRESS) {
			return -1;
		}
		
		real_machine -> cpu.ra = read_word(&(real_machine -> mem), real_machine -> ch_dev.of + MEM_BEG_SHARED_MEM);	

		real_machine -> cpu.ss = 0;
		return 0;
	}
	
	// BGxy xy must be between 0 - 1F and contained in OF 
	if(real_machine -> ch_dev.dt == SHARED_MEM && real_machine -> ch_dev.st == RA_REG) {
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

	}
	
	// GEDA 
	if(real_machine -> ch_dev.dt == RA_REG && real_machine -> ch_dev.st == IO_STREAM) {
		if(scanf("%x", &(real_machine -> cpu.ra)) != 1) {
			return -1;
		}
	
		return 0;
	}
	
	// PUTA 
	if(real_machine -> ch_dev.dt == IO_STREAM && real_machine -> ch_dev.st == RA_REG) {
		printf("%x", real_machine -> cpu.ra);	
	
		return 0;
	}
	
	// HDxy OF must contain the xy value
	if(real_machine -> ch_dev.dt == USER_MEM && real_machine -> ch_dev.st == DISK_MEM) {
		uint32_t count = real_machine -> ch_dev.cb / MEM_WORD_SIZE;
		uint8_t rem = real_machine -> ch_dev.cb % MEM_WORD_SIZE;
		uint32_t addr_hd = real_machine -> cpu.ra;
		if(addr_hd >= HD_MAX_HARD_DISK_ADDRESS){
			real_machine -> cpu.pi = RM_PI_INVALID_ADDRESS;
			return -1;
		}
		uint16_t v_addr = real_machine -> ch_dev.of; 		
		uint16_t r_addr;	
	
		for(uint32_t i = 0; i < count; ++i) {
			r_addr = translate_to_real_address(real_machine, v_addr, page_table_index);
			uint32_t word = read_word_hard_disk(&(real_machine -> hd), addr_hd);		
			// reverse endianess
			int temp = 1;
			if(*(char*)&temp == 1) {
			word = ((word >> 24) & 0x000000FF) | 
      				((word >>  8) & 0x0000FF00) |
       				((word <<  8) & 0x00FF0000) |
       				((word << 24) & 0xFF000000);
			}

			if(write_word(&(real_machine -> mem), r_addr, word) != 0) {
				return -1;
			}
			addr_hd += MEM_WORD_SIZE;
			v_addr += MEM_WORD_SIZE;
		}	
	
		if(rem != 0) {
			uint32_t word = 0;
			
			for(uint8_t i = 0; i < rem; ++i) {
				int temp = 1;
				if(*(char*)&temp == 1) {
					word |= (read_byte_hard_disk(&(real_machine -> hd), addr_hd) << (8 * (MEM_WORD_SIZE - 1 - i)));
				}
				else {
					word |= (read_byte_hard_disk(&(real_machine -> hd), addr_hd) << (8 * i));
				}
				++addr_hd;
			}
		
			if(write_word(&(real_machine -> mem), r_addr, word) != 0) {
				return -1;
			}
		}
	
		return 0;
	}
	
	// HPxy OF must contain the xy value
	if(real_machine -> ch_dev.dt == DISK_MEM && real_machine -> ch_dev.st == USER_MEM) {
		uint32_t count = real_machine -> ch_dev.cb / MEM_WORD_SIZE;
		uint8_t rem = real_machine -> ch_dev.cb % MEM_WORD_SIZE;
		uint32_t addr_hd = real_machine -> cpu.ra;
		if(addr_hd >= HD_MAX_HARD_DISK_ADDRESS){
			real_machine -> cpu.pi = RM_PI_INVALID_ADDRESS;
			return -1;
		}
		uint16_t v_addr = real_machine -> ch_dev.of;	
		uint16_t r_addr;
	
		for(uint32_t i = 0; i < count; ++i) {
			uint16_t r_addr = translate_to_real_address(real_machine, v_addr, page_table_index);
			uint32_t word = read_word(&(real_machine -> mem), r_addr);
			
			int temp = 1;
			if(*(char*)&temp == 1) {
			word = ((word >> 24) & 0x000000FF) | 
      				((word >>  8) & 0x0000FF00) |
       				((word <<  8) & 0x00FF0000) |
       				((word << 24) & 0xFF000000);
			}
			
			if(write_word_hard_disk(&(real_machine -> hd), addr_hd, word) != 0) {
				return -1;
			}
			addr_hd += MEM_WORD_SIZE;
			v_addr += MEM_WORD_SIZE;
		}
	
		if(rem != 0) {
			uint32_t word = read_word(&(real_machine -> mem), r_addr);
			for(uint8_t i = 0; i < rem; ++i) {
				int temp = 1;
				if(*(char*)&temp == 1) {	
					if(write_byte_hard_disk(&(real_machine -> hd), addr_hd, (word >> (8 * i)) & 0xff) != 0) {
					}
				}
				else {
					if(write_byte_hard_disk(&(real_machine -> hd), addr_hd, (word >> (8 * (MEM_WORD_SIZE - 1 - i))) & 0xff) != 0) {
						return -1;
					}
				}
			}
		}
		
		return 0;
	}
	
	// PSTR prints a char array from the addrress saved in RA, RC, (CB) says how many bytes x
	// doesnt work if the pages are not linear need to translate inside this function
	if(real_machine -> ch_dev.dt == IO_STREAM && real_machine -> ch_dev.st == USER_MEM) {
		uint32_t size = (real_machine -> ch_dev.cb) / MEM_WORD_SIZE;
		uint8_t rem = (real_machine -> ch_dev.cb) % MEM_WORD_SIZE;
		uint32_t addr = real_machine -> cpu.ra;
		//uint16_t r_addr = translate_to_real_address(real_machine, addr, page_table_index);	
		/*if(r_addr >= MEM_MAX_USER_ADDRESS) {
			real_machine -> cpu.pi = RM_PI_INVALID_ADDRESS;
			return -1;
		} 
		*/
	 
		if(rem != 0) {
			++size;
		}

		uint32_t byte_count = real_machine -> ch_dev.cb;
		for(uint32_t i = 0; i < size; ++i) {
			uint16_t r_addr = translate_to_real_address(real_machine, addr, page_table_index);	
			if(r_addr >= MEM_MAX_USER_ADDRESS) {
				real_machine -> cpu.pi = RM_PI_INVALID_ADDRESS;
				return -1;
			} 
	
			uint32_t word = read_word(&(real_machine -> mem), r_addr);
			addr += MEM_WORD_SIZE;
			for(uint8_t b = 0; b < MEM_WORD_SIZE && byte_count > 0; ++b) {
				uint8_t ch = (word >> (8 * (MEM_WORD_SIZE - 1 - b))) & 0xff;
				putchar(ch);
				--byte_count;
			}
		}
		
		return 0;
	}

	return 0;
}

void pi_si_reset(Real_machine* real_machine) {
	if(!real_machine) {
		return;
	}

	real_machine -> cpu.si = 0;
	real_machine -> cpu.pi = 0;
}

uint8_t check(Real_machine* real_machine) {
	if(!real_machine) {
		return 1;
	}

	if(real_machine -> cpu.ti > 10 || real_machine -> cpu.ti == 0) {
		real_machine -> cpu.ti = CPU_DEFAULT_TIMER_VALUE;
	} 

	return 0;
}
