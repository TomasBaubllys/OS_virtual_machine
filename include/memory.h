#ifndef MEMORY_H_INCLUDED
#define MEMORY_H_INCLUDED

#define _MEM_WORD_SIZE 4

#define _MEM_USER_MEMORY_SIZE 816
#define _MEM_SUPERVISOR_MEMORY_SIZE 64
#define _MEM_SHARED_MEMORY_SIZE 32

#include <string.h>

typedef struct Memory {
	uint32_t user[_MEM_USER_MEMORY_SIZE];
	uint32_t shared[_MEM_SHARED_MEM_SIZE];
	uint32_t supervisor[_MEM_SUPERVISOR_MEMORY_SIZE];

} Memory;

int init_memory(Memory* mem);
uint32_t read_word(Memory* mem); 					// not implemented maybe ad a mem_mode? as in user - supervisor
int write_word(Memory* mem, uint16_t address, uint32_t word); 		// not implemented yet

#endif
