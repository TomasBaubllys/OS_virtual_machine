#include "../include/real_machine.h"
#include "../include/virtual_machine.h"
#include <assert.h>

int main(int argc, char* argv[]) {
	Real_machine real_machine;

    	assert(init_real_machine(&real_machine) == 0);

    	while(1) {
	

    	}	
        
	return 0;
}
