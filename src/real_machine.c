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

void real_machine_run(Real_machine* real_machine, File_entry* file_entry) {
	if(!real_machine || !file_entry) {
		return;
	}

	if(file_entry >= MEM_MAX_USER_ADDRESS) {
		return;
	}

	Virtual_machine virtual_machine;
	if(init_virtual_machine(&virtual_machine, &real_machine -> cpu, &real_machine -> mem) != 0) {
		return;
	}

	// copy the program to super_visor_memory
	uint32_t copy_count = file_entry -> size / MEM_WORDS_SUPERVISOR_COUNT;
	uint32_t copy_count_rem = file_entry -> size % MEM_WORDS_SUPERVISOR_COUNT;
	
	for(int i = 0; i < copy_count; ++i) {
		// copy to supervisor memory
		real_machine -> ch_dev.dt = SUPER_MEM;
		real_machine -> ch_dev.db = MEM_BEG_SUPERVISOR_ADDR;
		real_machine -> ch_dev.cb = MEM_WORDS_SUPERVISOR_COUNT;
		real_machine -> ch_dev.st = HD_DISK;
		real_machine -> ch_dev.sb = ((file_entry -> offset / MEM_WORD_SIZE) / MEM_PAGE_SIZE) + i;	// calculate the hard disk page
		real_machine -> ch_dev.of = file_entry -> offset % (MEM_WORD_SIZE * MEM_PAGE_SIZE);
		xchg(&real_machine -> ch_dev);

		// check if the program starts with #LOS
		if(i == 0) {

		}

		// check if the program ends with #BYE
		if(i == copy_count - 1 && copy_count_rem == 0) {

		}
	}

	// check if the program ends with #BYE
	if(copy_count_rem != 0) {

	}
	
	// validate the program
	
	// if the program is valid copy from super_mem to user_mem

	real_machine -> cpu.pc = 0;

	while(1) {
		virtual_machine_execute(&virtual_machine);

		if(real_machine -> cpu.pi + real_machine -> cpu.si > 0 || real_machine -> cpu.ti == 0) {
			interupt(&real_machine -> cpu);
		}	
		
		check(&real_machine);
		
	}

	destroy_virtual_machine(&virtual_machine);
}