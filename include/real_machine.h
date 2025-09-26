#ifndef REAL_MACHINE_H_INCLUDED
#define REAL_MACHINE_H_INCLUDED

#include "cpu.h"
#include "memory.h"
#include "hard_disk.h"
#include "channel_device.h"
#include "util.h"

#define RM_VM_MAX_COUNT 3
#define RM_USER_VM_PAGE_INDEX_EXCEEDED 0xfffd

#define RM_SI_GEDA 1
#define RM_SI_PUTA 2
#define RM_SI_PSTR 3
#define RM_SI_LW 4
#define RM_SI_SW 5
#define RM_SI_BP 6
#define RM_SI_BG 7
#define RM_SI_HD 8
#define RM_SI_HP 9
#define RM_SI_STOP 10

#define RM_PI_INVALID_ADDRESS 1
#define RM_PI_INVALID_OPCODE 2
#define RM_PI_INVALID_ASSIGNMENT 3
#define RM_PI_OVERFLOW 4
#define RM_PI_DIVISION_BY_ZERO 5

#define RM_MSG_VM_LIMIT_REACHED "Maximum VM limit exceeded/n"
#define RM_MSG_FAILED_REMOVE_VM "Failed to remove virtual machine/n"
#define RM_MSG_SUCCESS_REMOVE_VM "Virtual machine successfully removed!\n"


typedef struct Virtual_machine Virtual_machine;

typedef struct Real_machine {
	CPU cpu;
	Memory mem;
	Hard_disk hd;
	Channel_device ch_dev; 
	Virtual_machine* vm;
	uint8_t vm_count;
} Real_machine;

int init_real_machine(Real_machine* real_machine); 

int execute_command(Real_machine* real_machine, uint8_t virtual_machine_index, uint32_t command);

int add_virtual_machine(Real_machine* real_machine, Virtual_machine* virtual_machine);

int remove_virtual_machine(Real_machine* real_machine, uint8_t virtual_machine_index);

// copies current register values from virtual to real machine
int copy_virtual_machine(Real_machine* real_machine, uint8_t virtual_machine_index);

// writes current real machine status to virtual machine
int write_virtual_machine(Real_machine* real_mahine, uint8_t virtual_machine_index);

uint16_t translate_to_real_address(Real_machine *real_machine, uint16_t address, uint8_t page_table_index);

int destroy_real_machine(Real_machine* real_machine);

int xchg(Real_machine* real_machine, uint8_t page_table_index);

void pi_si_reset(Real_machine* real_machine);

uint8_t check(Real_machine* real_machine);

#endif // REAL_MACHINE_H_INCLUDED
