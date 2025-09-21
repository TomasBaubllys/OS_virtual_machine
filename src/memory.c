#include "../include/memory.h"

int init_memory(Memory* mem) {
	if(!mem) {
		return -1;
	}	

	memset(mem -> memory, 0, sizeof(mem -> memory));	

	return 0;
}


uint32_t read_word(Memory* mem, const uint16_t address) {
	if(address >= MEM_MAX_ADDRESS - MEM_WORD_SIZE)	{
		fprintf(stderr, MEM_BAD_ADDRESS_ERR);
		return 0;
	}

	uint8_t rem = address % MEM_WORD_SIZE;
	uint16_t index = address / MEM_WORD_SIZE;

	if(rem == 0) {
		return mem -> memory[index];
	} 
	
	uint32_t word = 0;	
	
	if(address % MEM_WORD_SIZE == 1) {
		word = (mem -> memory[index]) << 8;
		word |= (mem -> memory[index + 1] & 0xff000000) >> 24; 
		return word;
	}
	
	if(address % MEM_WORD_SIZE == 2) {		
		word = (mem -> memory[index]) << 16;
		word |= (mem -> memory[index + 1] & 0xffff0000) >> 16; 
		return word;
	}
	
	if(address % MEM_WORD_SIZE == 3) {
		word = (mem -> memory[index]) << 24;
		word |= (mem -> memory[index + 1] & 0xffffff00) >> 8; 
		return word;
	}
} 			

int write_word(Memory* mem, const uint16_t address, const uint32_t word) {
	if(address >= MEM_MAX_ADDRESS - MEM_WORD_SIZE) {
		fprintf(stderr, MEM_BAD_ADDRESS_ERR);
		return -1;
	}
	
	uint8_t rem = address % MEM_WORD_SIZE;
	uint16_t index = address / MEM_WORD_SIZE;

	if(rem == 0) {
		mem -> memory[index] = word;
		return 0;
	}

	if(rem == 1) {
		mem -> memory[index] &= 0xff000000;
		mem -> memory[index] = (mem -> memory[index]) | (word >> 8);
		

		mem -> memory[index + 1] &= 0x00ffffff;
		mem -> memory[index + 1] |= word << 24;
		return 0;
	}	

	if(rem == 2) {
		mem -> memory[index] &= 0xffff0000;
		mem -> memory[index] = (mem -> memory[index]) | (word >> 16);
				
		mem -> memory[index + 1] &= 0x0000ffff;
		mem -> memory[index + 1] |= word << 16;
		return 0;
	}
	
	if(rem == 3) {
		mem -> memory[index] &= 0xffffff00;
		mem -> memory[index] |= word >> 24;
		
		mem -> memory[index + 1] &= 0x000000ff;
		mem -> memory[index + 1] |= word << 8;
		return 0;
	}
	
	return -1;
}


void fprint_memory(FILE* stream, Memory* mem, uint16_t start, uint16_t end, uint8_t column_count) {
	if(start >= end) {
		return;
	}
	
	// round both numbers to be divisible by 4 

	if(end >= MEM_MAX_ADDRESS) {
		fprintf(stream, MEM_BAD_ADDRESS_ERR);
	}	
	
	// take the floor
	start /= MEM_WORD_SIZE;
	
	if(end % MEM_WORD_SIZE != 0) {
		if(end >= MEM_MAX_ADDRESS - MEM_WORD_SIZE) {
			 end /= MEM_WORD_SIZE;
		}
		else {
			 end = (end / MEM_WORD_SIZE) + 1;
		}
	}
	else {
		end /= MEM_WORD_SIZE;
	}	
	
	uint8_t counter = column_count;
	fprintf(stream, "\n%*x :", MEM_MAX_ADDRESS_LENGTH, start * MEM_WORD_SIZE);

	// print the memory (optimize later)
	while(start <= end) {
		fprintf(stream, "%8x ", mem -> memory[start]);
		++start;
		--counter;
		
		if(counter <= 0) {
			fprintf(stream, "\n%*x :", MEM_MAX_ADDRESS_LENGTH, start * MEM_WORD_SIZE);
			counter = column_count;
		}
	}
}
