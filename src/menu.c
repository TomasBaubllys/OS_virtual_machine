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
			break;
		case LOAD_PROGRAM_TO_VM:
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

    printf("\n==== Virtual Machine Menu ====\n");
    printf("1) Add Virtual Machine\n");
    printf("2) Load Program into VM\n");
    printf("3) Run Virtual Machine\n");
    printf("4) Dump Memory\n");
    printf("5) Quit\n");
    printf("==============================\n");
    printf("Select option: ");

    if (scanf("%d", &choice) != 1) {
        while (getchar() != '\n'); 
        choice = 0;
    }

    return choice;
}

