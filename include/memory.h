#ifndef MEMORY_H_INCLUDED
#define MEMORY_H_INCLUDED

#define MEM_WORD_SIZE 4

#define MEM_USER_MEMORY_SIZE 816
#define MEM_SUPERVISOR_MEMORY_SIZE 64
#define MEM_SHARED_MEMORY_SIZE 32

#define MEM_TOTAL_MEMORY MEM_USER_MEMORY_SIZE + MEM_SHARED_MEM_SIZE + MEM_SUPERVISOR_MEMORY_SIZE

#define MEM_BAD_ADDRESS_ERR "Maximum address exceeded" 

#include <string.h>
#include <stdio.h>

typedef struct Memory {
	// uint32_t user[MEM_USER_MEMORY_SIZE];
	// uint32_t shared[MEM_SHARED_MEM_SIZE];
	// uint32_t supervisor[MEM_SUPERVISOR_MEMORY_SIZE];
	uint32_t memory[MEM_TOTAL_MEMORY]
} Memory;

int init_memory(Memory* mem);

uint32_t read_word(Memory* mem, const uint16_t address); 			

int write_word(Memory* mem, const uint16_t address, const uint32_t word); 
 
// prints memory from start address till end address, they must be aligned to a word a.k.a divisible by 4
void print_memory(Memory* mem, uint16_t start, uint16_t end);

#endif
