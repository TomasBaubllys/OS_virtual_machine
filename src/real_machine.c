#include "../include/real_machine.h"
#include "../include/virtual_machine.h"

int init_real_machine(Real_machine* real_machine) {
	if(!real_machine) {
		return -1;
	}

	if(init_cpu(&(real_machine -> cpu), &(real_machine -> mem), &(real_machine -> ch_dev)) != 0) {
		return -1;
	}

	if(init_memory(&(real_machine -> mem), &(real_machine -> cpu)) != 0) {
		return -1;
	}

	if(init_hard_disk(&(real_machine -> hd)) != 0) {
		return -1;
	}

	if(init_channel_device(&(real_machine -> ch_dev), &(real_machine -> mem), &(real_machine -> hd)) != 0) {
		return -1;
	}
	
	return 0;
};

int add_virtual_machine(Real_machine* real_machine, Virtual_machine* virtual_machine) {
	if(!real_machine || !virtual_machine) {
		return -1;
	} 

	real_machine -> vm = *virtual_machine;

	return 0;
}


// not implemented yet
int destroy_real_machine(Real_machine* real_machine) {
	if(!real_machine) {
		return -1;
	}	

	return 0;
}

int remove_virtual_machine(Real_machine* real_machine) {
	if(!real_machine) {
		return -1;
	}

	return 0;
}

void real_machine_run() {

}