#ifndef VIRTUAL_MACHINE_H_INCLUDED
#define VIRTUAL_MACHINE_H_INCLUDED

#include "real_machine.h"

#define VM_DEFAULT_PC_VAL 0
#define VM_VIRTUAL_MACHINE_BLOCK_COUNT 16
#define VM_MAX_VIRTUAL_ADDRESS 0x400
#define VM_MAX_VIRTUAL_ADDDRESS_EXCEEDED 0xffff
#define VM_REAL_MACHINE_NULL 0xfffe

#define VM_EXEC_FAILED_NULL_ERR "Virtual machine is null execution cancelled\n"

typedef struct Virtual_machine {
	uint16_t vm_pc;
	CPU* cpu;
	Memory* memory;
	uint16_t vm_arg;
} Virtual_machine;

int init_virtual_machine(Virtual_machine* virtual_machine, CPU* cpu, Memory* memory);

// loads a program begging from a virtual address virtual_address
int load_program_virtual_machine(Real_machine* real_machine, uint8_t virtual_machine_index, uint16_t virtual_address, uint32_t* program, uint16_t program_len);

int destroy_virtual_machine(Virtual_machine* virtual_machine);

void reset_virtual_machine_registers(Virtual_machine* virtual_machine);

void virtual_machine_execute(Virtual_machine* virtual_machine);

#endif // VIRTUAL_MACHINE_H_INCLUDED
