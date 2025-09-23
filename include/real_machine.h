#ifndef REAL_MACHINE_H_INCLUDED
#define REAL_MACHINE_H_INCLUDED

#include "cpu.h"
#include "memory.h"
#include "hard_disk.h"
#include "channel_device.h"
#include "util.h"

typedef struct Real_machine {
	CPU cpu;
	Memory mem;
	Hard_disk hd;
	Channel_device ch_dev; 
} Real_machine;

int init_real_machine(Real_machine* real_machine); 

int execute_command(Real_machine* real_machine, uint32_t command);

#endif // REAL_MACHINE_H_INCLUDED
