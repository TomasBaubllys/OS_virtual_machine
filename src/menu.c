#include "../include/menu.h"


uint8_t menu(Real_machine* real_machine) {
	if(!real_machine) {
		fprintf(stderr, MENU_NULL_REAL_MACHINE_ERR);
		exit(MENU_NULL_REAL_MACHINE_EXIT_CODE);
	}	

	uint8_t choice = display_menu();
	while(choice > MAX_CHOICE || choice == 0) {
		choice = display_menu();
	}

	switch(choice) {
		case ADD_VM:
			return ADD_VM;
			break;
		case LOAD_PROGRAM_TO_VM:
			File_entry file_choice = print_files(&(real_machine->hd));
			uint8_t virtual_machine_index = select_virtual_machine(real_machine);
			if(virtual_machine_index > real_machine -> vm_count) {
				// return to menu, maybe print a message
				return MENU_ON;
			} 
			
			uint32_t size = 0;
			uint32_t* program = read_program(&(real_machine -> hd), &file_choice, &size);
			if(!program) {
				
				printf(MSG_FAILED_TO_READ_PROGRAM);
				return MENU_ON;
		
			}
			
			// reverse the endianess :((
			int temp = 1;
			if(*(char*)&temp == 1) {
				reverse_endianness_array(program, size);	
			}

			if(load_program_virtual_machine(real_machine, virtual_machine_index, VM_DEFAULT_PC_VAL, program, size) != 0) {
				printf(MSG_FAILED_TO_LOAD_PROGRAM);
				free(program);
				return MENU_ON;
			}
			
			printf(MSG_PROGRAM_LOAD_SUCCESS);
			free(program);				
			return MENU_ON;					
			break;
		case RUN_VM:
			return RUN_VM;
		case DUMP_MEM:
			dump_memory(real_machine);
			return MENU_ON;
		case QUIT:
			return QUIT;
		default:
			break;
			
	}
	
	return MENU_ON;
}

void dump_memory(Real_machine* real_machine) {
	if(!real_machine) {
		return;
	}
	
	fprint_memory(stdout, &(real_machine -> mem), 0, MEM_MAX_ADDRESS - 1, MENU_MEM_DUMP_COL_COUNT);
}

int display_menu(void) {
    int choice = 0;

    printf(MSG_MENU);
    printf("1) " MSG_ADD_VM);
    printf("2) " MSG_LOAD_PROGRAM);;
    printf("3) " MSG_START_VM);
    printf("4) " MSG_SHOW_MEMORY);
    printf("5) " MSG_QUIT);
    printf("==============================\n");
    printf(MSG_SELECT_OPTION);

    if (scanf("%d", &choice) != 1) {
        while (getchar() != '\n'); 
        choice = 0;
    }

    return choice;
}

int select_virtual_machine(Real_machine* real_machine) {
	if(!real_machine) {
		// add error message later
		exit(-1);
	}
	
	printf("Select a vm\n");
	for(uint8_t i = 0; i < real_machine -> vm_count; ++i) {
		printf("%d) vm no. %d\n", i, i + 1);
	}
	
	int choice = 0;
	if(scanf("%d", &choice) != 1) {
		while(getchar() != '\n');
		choice = -1;
	}

	return choice;
} 
