/* Virtual - Real machine (OS2)
 * Authors: Tomas Baublys and Goda Gutparakyte
 * */

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

	Virtual_machine virtual_machine;
	
	// assume the machine is loaded
	while(1) {
		virtual_machine_execute(&virtual_machine);

		if(real_machine.cpu.pi + real_machine.cpu.si > 0) {
			interupt(&real_machine.cpu);
		}	
		
		check(&real_machine);
		
	}
        
	return 0;
}
