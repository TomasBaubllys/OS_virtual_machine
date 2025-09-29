#include "../include/virtual_machine.h"
#include "../include/menu.h"
#include <assert.h>

int main(int argc, char* argv[]) {
	Real_machine real_machine;
    	assert(init_real_machine(&real_machine) == 0);
	uint8_t menu_status = MENU_ON;
	uint8_t running = 0;
	int vm_index = 0;
	
	uint8_t stepping = 0;
	
	while(1) {
		switch(menu_status) {
			case MENU_ON:
				menu_status = menu(&real_machine);
				break;
			case RUN_VM:
				if(running == 0) {
					if(real_machine.vm_count == 0){
						printf("%s", MSG_NO_VMS_ADDED);
						menu_status = MENU_ON;
						break;
					}
					vm_index = select_virtual_machine(&real_machine);
				
					if(vm_index < 0 || vm_index > RM_VM_MAX_COUNT || vm_index == -1) {
						menu_status = MENU_ON;
						break;
					}

					printf(MSG_CHOOSE_RUN_MODE);
					int choice = 0;

					if (scanf("%d", &choice) != 1) {
        				while (getchar() != '\n'); 
        				choice = -1;
    				}

					if(choice == 2){
						stepping = 1;
					}
					else{
						stepping = 0;
					}
					
					running = 1; 
					break;
				}		
				else {
					

					copy_virtual_machine(&(real_machine), vm_index);
					uint32_t command = read_word(&(real_machine.mem), real_machine.cpu.pc);
					execute_command(&(real_machine), vm_index, command);
					
					if(stepping == 1) {
						uint32_t next_comm_addr = translate_to_real_address(&real_machine, real_machine.vm[vm_index].pc, real_machine.vm[vm_index].page_table_index);
						printf("Next command: %x\n", read_word(&(real_machine.mem), next_comm_addr));
						printf("PC: %x\nPI: %x\nSI: %x\nTR: %x\nTI: %x\nSF: %x\nMR: %x\nSS: %x\nRA: %x\nRB: %x\nRC: %x\n", 
							real_machine.cpu.pc, real_machine.cpu.pi, real_machine.cpu.si, real_machine.cpu.tr, real_machine.cpu.ti, real_machine.cpu.sf,
							real_machine.cpu.mr, real_machine.cpu.ss, real_machine.cpu.ra, real_machine.cpu.rb, real_machine.cpu.rc);
						printf(MSG_ANY_KEY);
						getchar();
					}
					
					--real_machine.cpu.ti;
					if(real_machine.cpu.pi + real_machine.cpu.si > 0) {
						if(real_machine.cpu.si == RM_SI_STOP) {
							pi_si_reset(&real_machine);
							running = 0;
							real_machine.cpu.mr = CPU_USER_MODE;
							menu_status = MENU_ON;
							break;
						}

						if(real_machine.cpu.si > 0) {
							real_machine.cpu.mr = CPU_SUPERVISOR_MODE;
							if(xchg(&real_machine, real_machine.vm[vm_index].page_table_index) != 0) {
							}
							real_machine.cpu.mr = CPU_USER_MODE;
			
						}
		
						pi_si_reset(&real_machine);
						real_machine.cpu.ti -= 3;
					}
					
					write_virtual_machine(&real_machine, vm_index);
					check(&real_machine);
    				break;
				}
			case QUIT:
				destroy_real_machine(&real_machine);
				exit(0);
			case ADD_VM:
				Virtual_machine virtual_machine;

				if(init_virtual_machine(&real_machine, &virtual_machine) == -1){
					printf(RM_MSG_VM_LIMIT_REACHED);
					menu_status = MENU_ON;
					break;
				}

				if(add_virtual_machine(&real_machine, &virtual_machine) == -1){
					printf(RM_MSG_VM_LIMIT_REACHED);
					menu_status = MENU_ON;
					break;
				}

				printf(MSG_VM_ADDED_SUCCESSFULLY);
				printf(MSG_VM_COUNT "%d", real_machine.vm_count);
				printf("\n");
				menu_status = MENU_ON;
				break;
			default:
				exit(-1);
		}
	}			
        
	return 0;
}
