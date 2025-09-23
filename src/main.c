#include "../include/real_machine.h"
#include <assert.h>

int main(int argc, char* argv[]) {
	Real_machine real_machine;

	assert(init_real_machine(&real_machine) == 0);
	
	uint32_t com1 = 0x4D4F6666;
	uint32_t com2 = 0x41506666;
	// LRrb
	uint32_t com3 = 0x4C527262;
	// LRpc
	uint32_t com4 = 0x4C527063;
	// MOee 
	uint32_t com5 = 0x4D4F6565;
	// LLra
	uint32_t com6 = 0x4C4C7261;
	// ADra
	uint32_t com7 = 0x41447261;
	// SBra
	uint32_t com8 = 0x53427261;
	// MO47
	uint32_t com9 = 0x4D4F3437;
	// MUra
	uint32_t com10 = 0x4D557261;
	// DVra
	uint32_t com11 = 0x44567262;


	execute_command(&real_machine, com1);
	execute_command(&real_machine, com2);
	execute_command(&real_machine, com3);

	assert(real_machine.cpu.ra = 510);
	assert(real_machine.cpu.rb = 510);

	assert(execute_command(&real_machine, com4) == -1);

	execute_command(&real_machine, com5);
	execute_command(&real_machine, com6);

	assert(real_machine.cpu.ra = 238);
	assert(real_machine.cpu.rb = 238);

	execute_command(&real_machine, com7);
	assert(real_machine.cpu.ra == 476);

	execute_command(&real_machine, com8);
	assert(real_machine.cpu.ra == 0);

	execute_command(&real_machine, com9);
	assert(real_machine.cpu.ra == 71);

	execute_command(&real_machine, com10);
	assert(real_machine.cpu.ra == 5041);

	printf("%d\n", real_machine.cpu.ra);
	printf("%d\n", real_machine.cpu.rb);

	execute_command(&real_machine, com11);
	printf("%d\n", real_machine.cpu.ra);
	printf("%d\n", real_machine.cpu.rb);
	assert(real_machine.cpu.ra == 5041 / 238);
	assert(real_machine.cpu.rb == 5041 % 238);
	
	printf("%d\n", real_machine.cpu.ra);
	printf("%d\n", real_machine.cpu.rb);
		
	return 0;
}
