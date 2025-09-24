#include "../include/real_machine.h"
#include "../include/virtual_machine.h"
#include <assert.h>

int main(int argc, char* argv[]) {
	Real_machine real_machine;
    	assert(init_real_machine(&real_machine) == 0);

	Virtual_machine vm;
	init_virtual_machine(&real_machine, &vm);

	assert(add_virtual_machine(&real_machine, &vm) == 0);

	// test program
	uint32_t test_program[] = {0x4D4F3032, 0x4C525242, 0x4D4F3066, 0x4D555242}; // after this ra register should 30 (2 * f)

	load_program_virtual_machine(&real_machine, 0, 0, test_program, 4);
	
	fprint_memory(stdout, &(real_machine.mem), 0, MEM_MAX_ADDRESS - 1, 16);
	
	// execute first 4 commands of vm1	
	assert(copy_virtual_machine(&real_machine, 0) == 0);
	execute_command(&real_machine, 0, read_word(&real_machine.mem, real_machine.cpu.pc));
	assert(write_virtual_machine(&real_machine, 0) == 0);
	
	copy_virtual_machine(&real_machine, 0);
	execute_command(&real_machine, 0, read_word(&real_machine.mem, real_machine.cpu.pc));
	write_virtual_machine(&real_machine, 0);    	
	
	copy_virtual_machine(&real_machine, 0);
	execute_command(&real_machine, 0, read_word(&real_machine.mem, real_machine.cpu.pc));
	write_virtual_machine(&real_machine, 0);    	
	
	copy_virtual_machine(&real_machine, 0);
	execute_command(&real_machine, 0, read_word(&real_machine.mem, real_machine.cpu.pc));
	write_virtual_machine(&real_machine, 0);    	
	
	printf("after program cpu.ra = %x\n", real_machine.cpu.ra);
	
	/*while(1) {
	

    	}*/	
        
	return 0;
}
