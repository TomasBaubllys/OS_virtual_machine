#include "../include/menu.h"

void menu(Real_machine* real_machine) {
	while(1){
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
				break;

			case STEPPING:
				int choice1;
				while(1){
					printf(MSG_STEPPING_CHOICE);
					choice1 = 0;
					
					if(scanf("%d", &choice1) != 1) {
						while(getchar() != '\n');
						continue;
					}

					if(choice1 == 1 || choice1 == 2){
						break;
					}
					else{
						printf(MSG_STEPPING_LOGGING_INVALID_CHOICE);
					}

				}

				if(choice1 == 1){
					real_machine -> cpu.sf |= 0x0008;
				}

				break;


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

				break;

			case DUMP_MEM:
				dump_memory(real_machine);
				printf("\n");
				break;
			case QUIT:
				return;
			default:
				break;
				
		}
	}
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
