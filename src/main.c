#include "../include/memory.h"
#include "../include/cpu.h"

int main(int argc, char* argv[]) {
	CPU cpu;
	Memory memory;
	
	init_cpu(&cpu);
	init_memory(&memory);	

	while(1) {
		// main loop
	}

	return 0;
}
