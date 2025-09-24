#include "../include/real_machine.h"
#include "../include/virtual_machine.h"
#include <assert.h>

int main(int argc, char* argv[]) {
	Real_machine real_machine;
    	assert(init_real_machine(&real_machine) == 0);

	Virtual_machine vm;
	init_virtual_machine(&real_machine, &vm);

	real_machine.vm[0] = vm;

	// test program
	uint32_t test_program[] = {0x4D4F3032, 0x4C525242, 0x4D4F3066, 0x4D555242}; // after this ra register should 30 (2 * f)

	load_program_virtual_machine(&real_machine, 0, 0, test_program, 4);
	
	fprint_memory(stdout, &(real_machine.mem), 0, MEM_MAX_ADDRESS, 16);
	
    	/*while(1) {
	

    	}*/	
        
	return 0;
}
