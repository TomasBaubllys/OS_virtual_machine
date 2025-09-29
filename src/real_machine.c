#include "../include/real_machine.h"
#include "../include/virtual_machine.h"

int init_real_machine(Real_machine* real_machine) {
	if(!real_machine) {
		return -1;
	}

	if(init_cpu(&(real_machine -> cpu)) != 0) {
		return -1;
	}

	if(init_memory(&(real_machine -> mem)) != 0) {
		return -1;
	}

	if(init_hard_disk(&(real_machine -> hd)) != 0) {
		return -1;
	}

	if(init_channel_device(&(real_machine -> ch_dev)) != 0) {
		return -1;
	}
	
	// allocate space for RM_VM_MAX_COUNT virtual machines
	real_machine -> vm = malloc(sizeof(Virtual_machine) * RM_VM_MAX_COUNT);
		
	real_machine -> vm_count = 0;
	
	return 0;
};

int add_virtual_machine(Real_machine* real_machine, Virtual_machine* virtual_machine) {
	if(!real_machine || !virtual_machine) {
		return -1;
	} 

	if(real_machine -> vm_count >= RM_VM_MAX_COUNT) {
		return -1;
	}

	real_machine -> vm[real_machine -> vm_count] = *virtual_machine;
	++(real_machine -> vm_count);
	return 0;
}


// not implemented yet
int destroy_real_machine(Real_machine* real_machine) {
	if(!real_machine) {
		return -1;
	}	
	
	close_hard_disk(&(real_machine -> hd));
	free(real_machine -> vm);
	real_machine -> vm = NULL;

	return 0;
}

int remove_virtual_machine(Real_machine* real_machine, uint8_t virtual_machine_index) {
	if(!real_machine) {
		return -1;
	}

	if(virtual_machine_index >= real_machine -> vm_count) {
		return -1;
	}

	if(destroy_virtual_machine(real_machine, &(real_machine -> vm[virtual_machine_index])) != 0) {
		return -1;
	}
	
	// move reposition other machines in its place
	for(uint8_t i = virtual_machine_index; i < real_machine -> vm_count - 1; ++i) {
		real_machine -> vm[i] = real_machine -> vm[i + 1];
	}

	--(real_machine -> vm_count);

	return 0;
}


void pi_si_reset(Real_machine* real_machine) {
	if(!real_machine) {
		return;
	}

	real_machine -> cpu.si = 0;
	real_machine -> cpu.pi = 0;
}

uint8_t check(Real_machine* real_machine) {
	if(!real_machine) {
		return 1;
	}

	if(real_machine -> cpu.ti > 10 || real_machine -> cpu.ti == 0) {
		real_machine -> cpu.ti = CPU_DEFAULT_TIMER_VALUE;
	} 

	return 0;
}
