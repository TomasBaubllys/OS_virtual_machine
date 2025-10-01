#ifndef CPU_H_INCLUDED
#define CPU_H_INCLUDED

#include <stdint.h>
#include "util.h"
#include "channel_device.h"

#define CPU_DEFAULT_TIMER_VALUE 10
#define CPU_USER_MODE 0
#define CPU_SUPERVISOR_MODE 1

#define CPU_DEFAULT_PC_VALUE 0x0000

#define CPU_UNKNOWN_REGISTER 0xff

#define RA_CODE 0x7261
#define RB_CODE 0x7262
#define RC_CODE 0x7263

#define CPU_PI_INVALID_ADDRESS 1
#define CPU_PI_INVALID_OPCODE 2
#define CPU_PI_INVALID_ASSIGNMENT 3
#define CPU_PI_OVERFLOW 4
#define CPU_PI_DIVISION_BY_ZERO 5

#define CPU_SI_GEDA 1
#define CPU_SI_PUTA 2
#define CPU_SI_PSTR 3
#define CPU_SI_LW 4
#define CPU_SI_SW 5
#define CPU_SI_BP 6
#define CPU_SI_BG 7
#define CPU_SI_STOP 8

#define INTERUPT_STOP -1
#define SEMAFOR_BUSY 1
#define SEMAFOR_FREE 0

typedef struct CPU {
	uint16_t pc;			// program counter
	uint16_t pi;			// program interupt register
	uint16_t si;			// supervisor interupt register
	uint16_t tr;			// page table register

	uint8_t ti;				// timer register
	uint8_t sf;				// flag register
	uint8_t mr;				// mode register
	uint8_t ss;				// semafor register

	uint32_t ra;			// general purpose register
	uint32_t rb;			// general purpose register
	uint32_t rc;			// general purpose register, also used for loops
	uint16_t ptr;

	Channel_device* channel_device;
	Memory* memory;
} CPU;

int init_cpu(CPU* cpu, Memory* memory, Channel_device* channel_device);

int interupt(CPU* cpu);

uint8_t check(CPU* cpu);

#endif // CPU_H_INCLUDED
