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

	return 0;
}
