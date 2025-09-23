#ifndef VIRTUAL_MACHINE_H_INCLUDED
#define VIRTUAL_MACHINE_H_INCLUDED

#include "real_machine.h"

#define VM_VIRTUAL_MACHINE_BLOCK_COUNT 16

typedef struct Virtual_machine {
	uint32_t ra;
	uint32_t rb;
	uint32_t rc;
	uint16_t pc;
	uint8_t sf;
    	
	Real_machine* rm;
	uint8_t status; // 1 - running, 0 - stopped (destroy the machine)
	uint8_t page_table_index;
} Virtual_machine;

int init_virtual_machine(Real_machine* real_machine, Virtual_machine* virtual_machine);

int destroy_virtual_machine(Virtual_machine* virtual_machine);

#endif // VIRTUAL_MACHINE_H_INCLUDED
