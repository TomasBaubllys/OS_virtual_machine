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
	uint32_t test_program[] = {0x4D4f3132,  // move 12 to ra 0x0
				0x4c525243, // move ra to rc
				0x4d4f3134, // move string address to ra
				0x50535452, // call the interrupt
				0x53544f50, // stop
				0x48454c4c, // HELL
				0x4f20574f, // O WO
				0x524c4421}; // RLD!

	load_program_virtual_machine(&real_machine, 0, 0, test_program, 8);
	
	fprint_memory(stdout, &(real_machine.mem), 0, MEM_MAX_ADDRESS - 1, 3);
	
	for(int i = 0; i < 8; ++i) {
		copy_virtual_machine(&(real_machine), 0);
		uint32_t command = read_word(&(real_machine.mem), real_machine.cpu.pc);
		execute_command(&(real_machine), 0, command);
				
		if(real_machine.cpu.pi + real_machine.cpu.si > 0) {
			if(real_machine.cpu.pi == RM_SI_STOP) {
				exit(1);
			}
			if(xchg(&real_machine) != 0) {
				// do something
				// kill the vm
				// if stop encoutered also kill the vm
			}
			
			pi_si_reset(&real_machine);
		}
		
		write_virtual_machine(&real_machine, 0);	
    	}	
        
	return 0;
}
