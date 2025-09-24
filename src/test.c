#include "../include/real_machine.h"
#include "../include/virtual_machine.h"
#include <assert.h>

int test_main(int argc, char* argv[]) {
	Real_machine real_machine;
	assert(init_real_machine(&real_machine) == 0);

	Virtual_machine vm1;
	assert(init_virtual_machine(&real_machine, &vm1) == 0);

	Virtual_machine vm2;
	assert(init_virtual_machine(&real_machine, &vm2) == 0);
	
	Virtual_machine vm3;
	assert(init_virtual_machine(&real_machine, &vm3) == 0);

	Virtual_machine vm4;
	assert(init_virtual_machine(&real_machine, &vm4) == -1);
	
	assert(destroy_virtual_machine(&real_machine, &vm2) == 0);
	assert(destroy_virtual_machine(&real_machine, &vm3) == 0);
	
	Virtual_machine vm5;
	assert(init_virtual_machine(&real_machine, &vm5) == 0);
	
	Virtual_machine vm6;
	assert(init_virtual_machine(&real_machine, &vm6) == 0);
	//fprint_memory(stdout, &(real_machine.mem), 0, MEM_MAX_ADDRESS - 1, 16);	

	// MOff
	uint32_t com1 = 0x4D4F6666;
	// APff
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
	// XRrb
	uint32_t com12 = 0x58527262;
	// ANrb
	uint32_t com13 = 0x414E7262;
	// NOra
	uint32_t com14 = 0x4E4F7261;
	// MO2b
	uint32_t com15 = 0x4D4F3262;
	// CM2b
	uint32_t com16 = 0x434D3262;
	// CRrb
	uint32_t com17 = 0x43527262;
	// JZ3a
	uint32_t com18 = 0x4A5A3361;
	// JA5b
	uint32_t com19 = 0x4A413562;
	// JA99
	uint32_t com20 = 0x4A423939;
	// JN82
	uint32_t com21 = 0x4a4e3832;
	// STOP
	uint32_t com22 = 0x53544F50;

	add_virtual_machine(&real_machine, &vm1);

	execute_command(&real_machine, 0, com1);
	execute_command(&real_machine, 0, com2);
	execute_command(&real_machine, 0, com3);


	assert(real_machine.cpu.ra == 510);
	assert(real_machine.cpu.rb == 510);

	assert(execute_command(&real_machine, 0, com4) == -1);

	execute_command(&real_machine, 0, com5);
	execute_command(&real_machine, 0, com6);

	assert(real_machine.cpu.ra == 238);
	assert(real_machine.cpu.rb == 238);

	execute_command(&real_machine, 0, com7);
	assert(real_machine.cpu.ra == 476);

	execute_command(&real_machine, 0, com8);
	assert(real_machine.cpu.ra == 0);

	execute_command(&real_machine, 0, com9);
	assert(real_machine.cpu.ra == 71);

	execute_command(&real_machine, 0, com10);
	assert(real_machine.cpu.ra == 5041);

	execute_command(&real_machine, 0, com11);
	assert(real_machine.cpu.ra == 5041 / 238);
	assert(real_machine.cpu.rb == 5041 % 238);

	execute_command(&real_machine, 0, com12);
	assert(real_machine.cpu.ra == (21 ^ 43));

	execute_command(&real_machine, 0, com13);
	assert(real_machine.cpu.ra == (62 & 43));

	execute_command(&real_machine, 0, com14);
	assert(real_machine.cpu.ra == ~42);

	execute_command(&real_machine, 0, com15);
	execute_command(&real_machine, 0, com16);
	assert(((real_machine.cpu.sf & 0x0002) >> 1) == 1);
	assert((real_machine.cpu.sf & 0x0001) == 0);

	execute_command(&real_machine, 0, com8);
	execute_command(&real_machine, 0, com17);
	assert(((real_machine.cpu.sf & 0x0002) >> 1) == 0);
	assert((real_machine.cpu.sf & 0x0001) == 0);


	execute_command(&real_machine, 0, com18);
	assert((real_machine.vm[0].pc != 0x003a));

	execute_command(&real_machine, 0, com19);
	assert(real_machine.vm[0].pc == 0x005b);


	execute_command(&real_machine, 0, com1);
	execute_command(&real_machine, 0, com17);
	execute_command(&real_machine, 0, com20);

	assert(real_machine.vm[0].pc == 0x0099);

	execute_command(&real_machine, 0, com17);
	execute_command(&real_machine, 0, com21);
	assert(real_machine.vm[0].pc == 0x0082);

	execute_command(&real_machine, 0, com22);
	assert(real_machine.cpu.si == 4);

	
	printf("%u\n", real_machine.cpu.ra);
	printf("%u\n", real_machine.cpu.rb);	
	printf("here\n");
	
	return 0;
}
