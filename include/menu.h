#ifndef MENU_H_INCLUDED
#define MENU_H_INCLUDED

#include "real_machine.h"
#include "virtual_machine.h"
#include "file.h"
#define MENU_ON 0
//#define ADD_VM 1  
//#define LOAD_PROGRAM 2
#define RUN_VM 1
#define STEPPING 2
#define LOGGING 3
#define DUMP_MEM 4
//#define DELETE_VM 5
#define QUIT 5


#define MAX_CHOICE 5

#define MENU_NULL_REAL_MACHINE_ERR "Real machine is null, exiting..."
#define MENU_NULL_REAL_MACHINE_EXIT_CODE -1

#define MENU_MEM_DUMP_COL_COUNT 16

// #define MSG_MENU "\n VIRTUAL MACHINE MENU \n"
#define MSG_MENU "\n\033[31m V\033[33m I\033[32m R\033[36m T\033[34m U\033[35m A\033[31m L\033[33m  \
\033[32m M\033[36m A\033[34m C\033[35m H\033[31m I\033[33m N\033[32m E\033[36m  \
\033[34m M\033[35m E\033[31m N\033[33m U\033[0m \n"

#define MSG_ADD_VM "Add a virtual machine\n"
#define MSG_REMOVE_VM "Remove virtual machine\n"
#define MSG_SHOW_MEMORY "Show memory\n"
#define MSG_LOAD_PROGRAM "Load a program\n"
#define MSG_CHOOSE_VM "Please choose a virtual machine:\n"
#define MSG_START_VM "Start a virtual machine\n"
#define MSG_SELECT_LOADED_VM "Please select a virtual machine: \n"
#define MSG_RUN_PROGRAM "Run program\n"
#define MSG_SELECT_LOADED_RPGRAMS "Please select a loaded program to run:\n"
#define MSG_STOP_VMI "Stop virtual machine\n"
#define MSG_RUN_PRGRM_NORMAL "Run program in normal mode\n"
#define MSG_RUN_PRGRM_TRACE "Run program in trace mode\n"
#define MSG_QUIT "Quit\n"
#define MSG_SELECT_OPTION "Please select an option: \n"
#define MSG_VM_ADDED_SUCCESSFULLY "Virtual machine added successfully\n"
#define MSG_VM_COUNT "Current virtual machine count: "
#define MSG_FAILED_TO_LOAD_PROGRAM "Failed to load the file to the virtual machine\n"
#define MSG_FAILED_TO_READ_PROGRAM "Failed to read the file from the hard disk\n"
#define MSG_PROGRAM_LOAD_SUCCESS "\033[0;32mProgram successfully loaded\033[0m\n"
#define MSG_NO_VMS_ADDED "No added virtual machines! Please add a VM. \n"
#define MSG_CHOOSE_RUN_MODE "Choose a mode: \n 1) Normal\n 2) Stepping\n"
#define MSG_ANY_KEY "Press enter to continue...\n"
#define MSG_SELECT_A_NUMBER_VM "Please select a number from 1 to %d \n"
#define MSG_DELETE_VM "Delete virtual machine\n"
#define MSG_STEPPING "Stepping \n"
#define MSG_LOGGING "Logging \n"
#define MSG_STEPPING_CHOICE "Choose a mode:\n 1. Stepping ON\n 2. Stepping OFF\n"
#define MSG_LOGGING_CHOICE "Choose a mode:\n 1. Logging ON\n 2. Logging OFF\n"
#define MSG_STEPPING_LOGGING_INVALID_CHOICE "Invalid choice, please select 1 or 2 \n"

// prints contents of memory
void dump_memory(Real_machine* real_machine);

// displays the main menu options
int display_menu();

// main menu loop
void menu(Real_machine* real_machine);

#endif // MENU_H_INCLUDED
