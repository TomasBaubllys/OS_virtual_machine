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
		case RUN_VM:
			// Select a file to run
			File_entry file_choice = print_files(&(real_machine -> hd));
			real_machine_run(real_machine, &file_choice);
			return MENU_ON;

		case STEPPING:
			int coice;
			while(1){
				printf(MSG_STEPPING_CHOICE);
				choice = 0;
				
				if(scanf("%d", &choice) != 1) {
					while(getchar() != '\n');
					continue;
				}

				if(choice == 1 || choice == 2){
					break;
				}
				else{
					printf(MSG_STEPPING_LOGGING_INVALID_CHOICE);
				}

			}

			if(choice == 1){
				real_machine -> cpu.sf |= 0x0008;
			}

			return MENU_ON;

		case LOGGING:
			int coice;
			while(1){
				printf(MSG_LOGGING_CHOICE);
				choice = 0;
				
				if(scanf("%d", &choice) != 1) {
					while(getchar() != '\n');
					continue;
				}

				if(choice == 1 || choice == 2){
					break;
				}
				else{
					printf(MSG_STEPPING_LOGGING_INVALID_CHOICE);
				}

			}

			if(choice == 1){
				real_machine -> cpu.sf |= 0x0010;
			}

			return MENU_ON;

		case DUMP_MEM:
			dump_memory(real_machine);
			printf("\n");
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
    //printf("1) " MSG_ADD_VM);
	//printf("2) " MSG_LOAD_PROGRAM);
    printf("1) " MSG_START_VM);
	printf("2) " MSG_STEPPING);
	printf("3) " MSG_LOGGING);
    printf("4) " MSG_SHOW_MEMORY);
	//printf("5) " MSG_DELETE_VM);
    printf("5) " MSG_QUIT);
    printf("==============================\n");
    printf(MSG_SELECT_OPTION);

	// pakeisti steppinga
	// pakeisti ar loginam (jei interruptas ismetam aks vyksta)
	// paleisti virtualia masina --> pasirinkti programa
	// quit

    if (scanf("%d", &choice) != 1) {
        while (getchar() != '\n'); 
        choice = 0;
    }

	printf("\n");
    return choice;
}

// int select_virtual_machine(Real_machine* real_machine) {
// 	if(!real_machine) {
// 		// add error message later
// 		exit(-1);
// 	}

// 	int choice = 0;

// 	while(1){
// 		printf(MSG_SELECT_LOADED_VM);
// 		for(uint8_t i = 0; i < real_machine -> vm_count; ++i) {
// 			printf("%d) vm no. %d\n", i + 1, i + 1);
// 		}
	
// 		choice = 0;
		
// 		if(scanf("%d", &choice) != 1) {
// 			while(getchar() != '\n');
// 			continue;
// 		}

// 		if (choice > 0 && choice <= real_machine -> vm_count){
// 			break;
// 		}
// 		else{
// 			printf(MSG_SELECT_A_NUMBER_VM, real_machine -> vm_count);

// 		}


// 	}
	
// 	printf("\n");

// 	return choice - 1;
// } 


		// case LOAD_PROGRAM:
		// 	if(real_machine->vm_count == 0){
		// 		printf(MSG_NO_VMS_ADDED);
		// 		return MENU_ON;
		// 	}

		// 	File_entry file_choice = print_files(&(real_machine -> hd));
		// 	uint8_t vm_index = select_virtual_machine(real_machine);
		// 	if(vm_index > (real_machine -> vm_count)) {
		// 		// return to menu, maybe print a message
		// 		return MENU_ON;
		// 	} 
			
		// 	uint32_t size = 0;
		// 	uint32_t* program = read_program(&(real_machine -> hd), &file_choice, &size);
		// 	if(!program) {
				
		// 		printf(MSG_FAILED_TO_READ_PROGRAM);
		// 		return MENU_ON;		
		// 	}
			
		// 	// reverse the endianess :((
		// 	int temp = 1;
		// 	if(*(char*)&temp == 1) {
		// 		reverse_endianness_array(program, size);	
		// 	}

		// 	if(load_program_virtual_machine(real_machine, vm_index, VM_DEFAULT_PC_VAL, program, size) != 0) {
		// 		printf(MSG_FAILED_TO_LOAD_PROGRAM);
		// 		free(program);
		// 		return MENU_ON;
		// 	}
			
		// 	printf(MSG_PROGRAM_LOAD_SUCCESS);
		// 	free(program);

		// 	return MENU_ON;