#ifndef VIRTUAL_MACHINE_H_INCLUDED
#define VIRTUAL_MACHINE_H_INCLUDED

#include "real_machine.h"

#define VM_DEFAULT_PC_VAL 0
#define VM_VIRTUAL_MACHINE_BLOCK_COUNT 16
#define VM_MAX_VIRTUAL_ADDRESS 0x400
#define VM_MAX_VIRTUAL_ADDDRESS_EXCEEDED 0xffff
#define VM_REAL_MACHINE_NULL 0xfffe

typedef struct Virtual_machine {
	uint32_t ra;
	uint32_t rb;
	uint32_t rc;
	uint16_t pc;
	uint8_t sf;
    	
	// Real_machine* rm;
	uint8_t status; // 1 - running, 0 - stopped (destroy the machine)
	uint8_t page_table_index;
} Virtual_machine;

int init_virtual_machine(Real_machine* real_machine, Virtual_machine* virtual_machine);

// loads a program begging from a virtual address virtual_address
int load_program_virtual_machine(Real_machine* real_machine, uint8_t virtual_machine_index, uint16_t virtual_address, uint32_t* program, uint16_t program_len);

int destroy_virtual_machine(Real_machine *real_machine, Virtual_machine* virtual_machine);

#endif // VIRTUAL_MACHINE_H_INCLUDED
