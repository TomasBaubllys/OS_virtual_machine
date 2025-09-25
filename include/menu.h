#ifndef MENU_H_INCLUDED
#define MENU_H_INCLUDED

#include "real_machine.h"
#define MENU_ON 0
#define ADD_VM 1  
#define LOAD_PROGRAM_TO_VM 2 
#define RUN_VM 3 
#define DUMP_MEM 4
#define QUIT 5

#define MAX_CHOICE 5

#define MENU_NULL_REAL_MACHINE_ERR "Real machine is null, exiting..."
#define MENU_NULL_REAL_MACHINE_EXIT_CODE -1

#define MENU_MEM_DUMP_COL_COUNT 16

void dump_memory(Real_machine* real_machine);

int display_menu();

uint8_t menu(Real_machine* real_machine);

#endif // MENU_H_INCLUDED

