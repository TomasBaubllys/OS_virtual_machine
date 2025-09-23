#include "../include/memory.h"

int init_memory(Memory* mem) {
	if(!mem) {
		return -1;
	}	

	memset(mem -> memory, 0, sizeof(mem -> memory));	
    	memset(mem -> used_pages, 0, sizeof(mem -> used_pages));
    
    	for(uint8_t i = 0; i < MEM_PAGE_COUNT; ++i) {
        	mem -> free_pages[i] = i;
    	}

    	mem -> used_page_count = 0;
    	mem -> free_page_count = MEM_PAGE_COUNT;

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


uint8_t get_free_page(Memory* mem) {
    if(mem -> free_page_count == 0) {
        return MEM_NO_FREE_PAGE_ERR;
    }

    // take the number from the back since its faster
    --(mem -> free_page_count);
    uint8_t free_page_num = mem -> free_pages[mem -> free_page_count];

    // and insert it into used pages also to the back
    if(mem -> used_page_count >= MEM_PAGE_COUNT - 1) {
        return MEM_INTERNAL_PAGING_ERR_MISMATCH_SIZES;
    }

    mem -> used_pages[mem -> used_page_count] = free_page_num;
    ++(mem -> used_page_count);

    return free_page_num;
}


int return_page(Memory* mem, uint8_t page_num) {
    if(page_num >= MEM_PAGE_COUNT) {
        return -1;
    } 

    if(mem -> used_page_count >= MEM_PAGE_COUNT) {
        return -1;
    }

    // find the page in used pages
    uint8_t index = 0;
    for(index; index < mem -> used_page_count; ++index) {
        if(mem -> used_pages[index] == page_num) {
            break;
        }
    }

    // check if the page was actually found or the cycle just ended
    if(index == MEM_PAGE_COUNT) {
        return -1;
    }

    // delete that page from the array
    for(uint8_t i = index; i < mem -> used_page_count - 1; ++i) {
        mem -> used_pages[i] = mem -> used_pages[i + 1];
    }

    --(mem -> used_page_count);

    // add the page to the back of free_pages
    if(mem -> free_page_count >= MEM_PAGE_COUNT) {
        return -1;
    }

    mem -> free_pages[mem -> free_page_count] = page_num;
    ++(mem -> free_page_count);

    return 0;
}
