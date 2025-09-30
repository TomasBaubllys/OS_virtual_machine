/* Virtual - Real machine (OS2)
 * Authors: Tomas Baublys and Goda Gutparakyte
 * */

#include "../include/virtual_machine.h"
#include "../include/menu.h"
#include <assert.h>

int main(int argc, char* argv[]) {
	Real_machine real_machine;
    assert(init_real_machine(&real_machine) == 0);
	menu(&real_machine);
    destroy_real_machine(&real_machine);
	return 0;
}
