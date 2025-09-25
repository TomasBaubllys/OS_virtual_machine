#include "../include/virtual_machine.h"
#include "../include/menu.h"
#include "../include/file.h"
#include <assert.h>

int main(int argc, char* argv[]) {
	Real_machine real_machine;
    	assert(init_real_machine(&real_machine) == 0);

	uint8_t menu_status = MENU_ON;

	while(1) {
		switch(menu_status) {
			case MENU_ON:
				menu_status = menu(&real_machine);
				break;
			case RUN_VM:
				copy_virtual_machine(&(real_machine), 0);
				uint32_t command = read_word(&(real_machine.mem), real_machine.cpu.pc);
				execute_command(&(real_machine), 0, command);
				
				if(real_machine.cpu.pi + real_machine.cpu.si > 0) {
					if(real_machine.cpu.si == RM_SI_STOP) {
						menu_status = MENU_ON;
						break;
					}
					if(real_machine.cpu.si > 0) {
						real_machine.cpu.mr = 1;
						if(xchg(&real_machine, real_machine.vm[0].page_table_index) != 0) {
						}
						real_machine.cpu.mr = 0;
			
					}
		
					pi_si_reset(&real_machine);
				}
		
				write_virtual_machine(&real_machine, 0);	
    				break;
			case QUIT:
				destroy_real_machine(&real_machine);
				exit(0);
			case ADD_VM:
				Virtual_machine virtual_machine;

				if(init_virtual_machine(&real_machine, &virtual_machine) == -1){
					printf("%s", RM_MSG_VM_LIMIT_REACHED);
					menu_status = MENU_ON;
					break;
				}

				if(add_virtual_machine(&real_machine, &virtual_machine) == -1){
					printf("%s", RM_MSG_VM_LIMIT_REACHED);
					menu_status = MENU_ON;
					break;
				}

				printf(MSG_VM_ADDED_SUCCESSFULLY);
				printf(MSG_VM_COUNT  "%d", real_machine.vm_count);
				menu_status = MENU_ON;
				break;
			default:
				exit(-1);
		}
	}			
        
	return 0;
}
