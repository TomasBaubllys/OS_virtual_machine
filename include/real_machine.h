#ifndef REAL_MACHINE_H_INCLUDED
#define REAL_MACHINE_H_INCLUDED

#include "cpu.h"
#include "memory.h"
#include "hard_disk.h"
#include "channel_device.h"
#include "util.h"

#define RM_VM_MAX_COUNT 3
#define RM_USER_VM_PAGE_INDEX_EXCEEDED 0xfffd

#define RM_MSG_VM_LIMIT_REACHED "Maximum VM limit exceeded\n"
#define RM_MSG_FAILED_REMOVE_VM "Failed to remove virtual machine\n"
#define RM_MSG_NO_VMS "No added VMs!\n"
#define RM_MSG_SUCCESS_REMOVE_VM "Virtual machine successfully removed!\n"
#define RM_MSG_BAD_PROGRAM "Invalid program encountered exiting...\n"

typedef struct Virtual_machine Virtual_machine;

typedef struct Real_machine {
	CPU cpu;
	Memory mem;
	Hard_disk hd;
	Channel_device ch_dev; 
	Virtual_machine* vm;
} Real_machine;

int init_real_machine(Real_machine* real_machine); 

int add_virtual_machine(Real_machine* real_machine, Virtual_machine* virtual_machine);

int destroy_real_machine(Real_machine* real_machine);

void real_machine_run(Real_machine* real_machine, File_entry* file_entry);

// validates if a code snippet in the supervisor memory looks valid
int real_machine_validate_supervisor(Real_machine* real_machine, uint32_t expected_program_length);

int load_program_supervisor(Real_machine* real_machine, File_entry* file_entry);

int load_program_user(Real_machine* real_machine, uint32_t program_length);

#endif // REAL_MACHINE_H_INCLUDED
