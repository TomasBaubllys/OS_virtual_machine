#ifndef REAL_MACHINE_H_INCLUDED
#define REAL_MACHINE_H_INCLUDED

#include "cpu.h"
#include "memory.h"
#include "hard_disk.h"
#include "channel_device.h"

typedef struct Real_machine {
	CPU cpu;
	Memory mem;
	Hard_disk hd;
	Channel_device ch_dev;
} Real_machine;

int init_real_machine(Real_machine* real_machine); 

#endif // REAL_MACHINE_H_INCLUDED
