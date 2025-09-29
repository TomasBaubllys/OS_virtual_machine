#include "../include/virtual_machine.h"

int init_virtual_machine(Real_machine* real_machine, Virtual_machine* virtual_machine) {
	if(!virtual_machine || !real_machine) {
		return -1;
	}
	
	// virtual_machine -> rm = real_machine;

	virtual_machine -> ra = 0;
	virtual_machine -> rb = 0;
	virtual_machine -> rc = 0;
	virtual_machine -> pc = 0;
	virtual_machine -> sf = 0;

	// machine is initialized and running
	virtual_machine -> status = 1;

	// get a page for the page table
	uint8_t page_table_index = get_free_page(&(real_machine -> mem));
	
	if(page_table_index == MEM_NO_FREE_PAGE_ERR) {
		return -1;
	}

	virtual_machine -> page_table_index = page_table_index; 
	
	// ask for 16 pages and write them to the page table
	uint8_t page_index;
	for(uint8_t i = 0; i < VM_VIRTUAL_MACHINE_BLOCK_COUNT; ++i) {
		page_index = get_free_page(&(real_machine -> mem));
		if(page_index == MEM_NO_FREE_PAGE_ERR) {
			// if ran out of pages return everything and return -1;
			for(uint8_t j = 0; j < i; ++j) {
				uint32_t pizza_slice = real_machine -> mem.memory[page_table_index * 16 + j];
				uint16_t real_page_addr = pizza_slice & 0x00ff;
				return_page(&(real_machine -> mem), real_page_addr);
			}	
			return_page(&(real_machine -> mem), page_table_index);
		
			return -1;
		}

		// else write it to the page table
		real_machine -> mem.memory[page_table_index * 16 + i] = (uint32_t)i << 16 | (uint32_t)page_index; 		
	}
	
	return 0;		
}

// return all the pages	
int destroy_virtual_machine(Real_machine *real_machine, Virtual_machine* virtual_machine) {
	if(!virtual_machine) {
		return -1;
	}


	for(uint8_t i = 0; i < VM_VIRTUAL_MACHINE_BLOCK_COUNT; ++i) {
	// printf("%d\n", virtual_machine -> rm -> mem.memory[(virtual_machine -> page_table_index) * 16 + i]);
		if(return_page(&(real_machine -> mem), (real_machine -> mem.memory[(virtual_machine -> page_table_index) * 16 + i]) & 0x00ff ) != 0) {
			return -1;
		} 
	}
	
	if(return_page(&(real_machine -> mem), virtual_machine -> page_table_index * 16) != 0) {
		return -1;
	}

	// officialy its deceased
	virtual_machine -> status = 0;

	return 0;
}

int load_program_virtual_machine(Real_machine* real_machine, uint8_t virtual_machine_index, uint16_t virtual_address, uint32_t* program, uint16_t program_len) {
	if(!real_machine) {
		return -1;
	}

	if(!program) {
		return -1;
	}

	if(virtual_machine_index >= RM_VM_MAX_COUNT) {
		return -1;
	}

	if(virtual_address >= VM_MAX_VIRTUAL_ADDRESS) {
		return -1;
	}	

	if(virtual_address + program_len * 4 >= VM_MAX_VIRTUAL_ADDRESS) {
		return -1;
	}

	uint16_t page_table_index = real_machine -> vm[virtual_machine_index].page_table_index;

	// write the program to memory
	for(uint16_t i = 0; i < program_len; ++i) {
		// translate virtual_address to a real one
		uint16_t real_address = translate_to_real_address(real_machine, virtual_address, page_table_index);	

		// write to that real address	
		write_word(&(real_machine -> mem), real_address, program[i]); 
	
		// increment virtual address by MEM_WORD_SIZE
		virtual_address += MEM_WORD_SIZE;
	}
	
	reset_virtual_machine_registers((real_machine -> vm) + virtual_machine_index);

	return 0;
}

void reset_virtual_machine_registers(Virtual_machine* virtual_machine) {
	if(!virtual_machine) {
		return;
	}

	virtual_machine -> ra = 0;
	virtual_machine -> rb = 0;
	virtual_machine -> rc = 0;
	virtual_machine -> pc = 0;
	virtual_machine -> sf = 0;
}
