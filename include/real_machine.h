#ifndef REAL_MACHINE_H_INCLUDED
#define REAL_MACHINE_H_INCLUDED

#include "cpu.h"
#include "memory.h"
#include "hard_disk.h"
#include "channel_device.h"
#include "util.h"

#define RM_VM_MAX_COUNT 3

typedef struct Virtual_machine Virtual_machine;

typedef struct Real_machine {
	CPU cpu;
	Memory mem;
	Hard_disk hd;
	Channel_device ch_dev; 
	Virtual_machine* vm;
} Real_machine;

int init_real_machine(Real_machine* real_machine); 

int execute_command(Real_machine* real_machine, uint32_t command);

// copies current register values from virtual to real machine
int copy_virtual_machine(Real_machine* real_machine, uint8_t virtual_machine_index);

// writes current real machine status to virtual machine
int write_virtual_machine(Real_machine* real_mahine, uint8_t virtual_machine_index);

// not implemented yet
int destroy_real_machine(Real_machine* real_machine);

#endif // REAL_MACHINE_H_INCLUDED
