#ifndef MENU_H_INCLUDED
#define MENU_H_INCLUDED

#include "real_machine.h"
#include "virtual_machine.h"
#include "file.h"
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

#define MSG_MENU "\n VIRTUAL MACHINE MENU \n"
#define MSG_ADD_VM "Add a virtual machine\n"
#define MSG_REMOVE_VM "Remove virtual machine\n"
#define MSG_SHOW_MEMORY "Show memory\n"
#define MSG_LOAD_PROGRAM "Load a program\n"
#define MSG_CHOOSE_VM "Please choose a virtual machine:\n"
#define MSG_START_VM "Start a virtual machine\n"
#define MSG_SELECT_LOADED_VM "Please select a virtual machine\n"
#define MSG_RUN_PROGRAM "Run program\n"
#define MSG_SELECT_LOADED_RPGRAMS "Please select a loaded program to run\n"
#define MSG_STOP_VMI "Stop virtual machine\n"
#define MSG_RUN_PRGRM_NORMAL "Run program in normal mode\n"
#define MSG_RUN_PRGRM_TRACE "Run program in trace mode\n"
#define MSG_QUIT "Quit\n"
#define MSG_SELECT_OPTION "Select option\n"
#define MSG_VM_ADDED_SUCCESSFULLY "Virtual machine added successfuly\n"
#define MSG_VM_COUNT "Current virtual machine count: \n"

void dump_memory(Real_machine* real_machine);

int display_menu();

uint8_t menu(Real_machine* real_machine);

int select_virtual_machine(Real_machine* real_machine); 

#endif // MENU_H_INCLUDED
