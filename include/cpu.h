#ifndef CPU_H_INCLUDED
#define CPU_H_INCLUDED

#include<stdint.h>

#define _CPU_DEFAULT_TIMER_VALUE 10
#define _CPU_USER_MODE 0
#define _CPU_SUPERVISOR_MODE 1

typedef struct CPU {
	uint16_t pc;
	uint16_t pi;
	uint16_t si;
	uint16_t tr;

	uint8_t ti;
	uint8_t sf;
	uint8_t mr;
	uint8_t ss;

	uint32_t ra;
	uint32_t rb;
	uint32_t rc;	
} CPU;

int init_cpu(CPU* cpu);

#endif
