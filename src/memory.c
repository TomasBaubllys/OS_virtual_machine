#include "../include/memory.h"

int init_memory(Memory* mem) {
	if(!mem) {
		return -1;
	}	

	// memset(mem -> user, 0, sizeof(mem -> user));
	// memset(mem -> shared, 0, sizeof(mem -> share));
	// memset(mem -> supervisor, 0, sizeof(mem -> supervisor));
	memset(mem -> memory, 0, sizeof(mem -> memory));	

	return 0;
}


uint32_t read_word(Memory* mem, const uint16_t address) {
	if(address > MEM_TOTAL_MEMORY)	{
		fprintf(stderr, MEM_BAD_ADDRESS_ERR);
		return 0;
	}
	
	return mem -> memory[address];
} 			

int write_word(Memory* mem, const uint16_t address, const uint32_t word) {
	if(address > MEM_TOTAL_MEMORY) {
		fprintf(stderr, MEM_BAD_ADDRESS_ERR);
		return -1;
	}
	
	mem -> memory[address] = word;

	return 0;
}
