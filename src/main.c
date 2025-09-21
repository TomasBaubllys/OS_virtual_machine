#include "../include/real_machine.h"
#include <assert.h>

int main(int argc, char* argv[]) {
	Real_machine real_machine;

	assert(init_real_machine(&real_machine) == 0);
	
	uint32_t com1 = 0x4D4F6666;
	uint32_t com2 = 0x41506666;

	execute_command(&real_machine, com1);
	execute_command(&real_machine, com2);

	assert(real_machine.cpu.ra = 510);
	
	printf("%d\n", real_machine.cpu.ra);
		
	return 0;
}
