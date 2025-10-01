#ifndef MEMORY_H_INCLUDED
#define MEMORY_H_INCLUDED

#define MEM_WORD_SIZE 4

#define MEM_USER_MEMORY_SIZE 816
#define MEM_SUPERVISOR_MEMORY_SIZE 64
#define MEM_SHARED_MEMORY_SIZE 32

#define MEM_TOTAL_MEMORY MEM_USER_MEMORY_SIZE + MEM_SHARED_MEMORY_SIZE + MEM_SUPERVISOR_MEMORY_SIZE

#define MEM_PAGE_COUNT 57
#define MEM_PAGE_SIZE 16
#define MEM_USER_PAGE_COUNT 51

#define MEM_MAX_ADDRESS 0xe40
#define MEM_MAX_USER_ADDRESS 0xcc0
#define MEM_MAX_USER_VM_ADDRESS 0x400
#define MEM_BEG_SHARED_MEM 0xcc0
#define MEM_END_SHARED_MEM 0xd40
#define MEM_MAX_SHARED_ADDRESS 0x80
#define MEM_MAX_ADDRESS_LENGTH 3
#define MEM_BAD_ADDRESS_ERR "Maximum address exceeded\n" 
#define MEM_BEG_SUPERVISOR_ADDR 0xd40
#define MEM_WORDS_SUPERVISOR_COUNT 64
#define MEM_SUPERVISOR_PAGE_COUNT 4
#define MEM_SUPERVISOR_PAGE_BEGIN 0x35

#define MEM_NO_FREE_PAGE_ERR 0xff
#define MEM_INVALID_PAGE_NUM_ERR 0xfe
#define MEM_INTERNAL_PAGING_ERR_MISMATCH_SIZES 0xfd

#define MEM_NULL_ADDR 0xffff

#include <string.h>
#include <stdio.h>
#include <stdint.h>
// #include "cpu.h"

typedef struct CPU CPU;

typedef struct Memory {
	uint32_t memory[MEM_TOTAL_MEMORY];
    uint8_t free_page_count;
    uint8_t free_pages[MEM_PAGE_COUNT];
    uint8_t used_page_count;
    uint8_t used_pages[MEM_PAGE_COUNT];
    CPU* cpu;
} Memory;

int init_memory(Memory* mem, CPU* cpu);

uint16_t translate_to_real_address(Memory* memory, uint16_t virtual_address);

uint32_t read_word(Memory* mem, const uint16_t address); 			

int write_word(Memory* mem, const uint16_t address, const uint32_t word); 

// returns the number of a free page, marks it as being used
uint8_t get_free_page(Memory* mem);

// returns the used page to free state
int return_page(Memory* mem, uint8_t page_num);

// prints memory from start address till end address, they must be aligned to a word a.k.a divisible by 4
void fprint_memory(FILE* stream, Memory* mem, uint16_t start, uint16_t end, uint8_t column_count);

#endif // MEMORY_H_INCLUDED
