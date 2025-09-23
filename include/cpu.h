#ifndef CPU_H_INCLUDED
#define CPU_H_INCLUDED

#include <stdint.h>
#include "../include/util.h"

#define CPU_DEFAULT_TIMER_VALUE 10
#define CPU_USER_MODE 0
#define CPU_SUPERVISOR_MODE 1

#define CPU_UNKNOWN_REGISTER 0xff

typedef struct CPU {
	uint16_t pc;			// program counter
	uint16_t pi;			// program interupt register
	uint16_t si;			// supervisor interupt register
	uint16_t tr;			// page table register

	uint8_t ti;			// timer register
	uint8_t sf;			// flag register
	uint8_t mr;			// mode register
	uint8_t ss;			// semafor register

	uint32_t ra;			// general purpose register
	uint32_t rb;			// general purpose register
	uint32_t rc;			// general purpose register, also used for loops
} CPU;

int init_cpu(CPU* cpu);

uint8_t get_register_num(uint16_t reg_char);

enum Registers {
	PC,
	PI,
	SI,
	TR,
	TI,
	SF,
	MR,
	SS,
	RA,
	RB,
	RC
};

#endif // CPU_H_INCLUDED
