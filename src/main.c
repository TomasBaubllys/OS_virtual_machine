#include "../include/memory.h"
#include "../include/cpu.h"
#include "../include/hard_disk.h"
#include <assert.h>

int main(int argc, char* argv[]) {
	CPU cpu;
	Memory memory;
	Hard_disk hard_disk;	

	assert(init_cpu(&cpu) == 0);
	assert(init_memory(&memory) == 0);	
	assert(init_hard_disk(&hard_disk) == 0);

	/*while(1) {
		// main loop
	}*/

	return 0;
}
