#include "../include/real_machine.h"

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

	return 0;
};
