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

	real_machine -> vm = virtual_machine;

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

	if(file_entry -> size >= MEM_MAX_USER_ADDRESS) {
		return;
	}

	Virtual_machine virtual_machine;
	if(init_virtual_machine(&virtual_machine, &real_machine -> cpu, &real_machine -> mem) != 0) {
		return;
	}

	// copy the program to super_visor_memory
	// copy the first time
	uint32_t bytes_to_copy_super_mem = file_entry -> size;
	// do not copy #LOS and #BYE to user mem
	uint32_t bytes_to_copy_user_mem = bytes_to_copy_super_mem - (MEM_WORD_SIZE * 2);
	uint32_t user_mem_dest_page = 0;

	real_machine -> ch_dev.dt = SUPER_MEM;
	real_machine -> ch_dev.db = MEM_BEG_SUPERVISOR_ADDR;
	real_machine -> ch_dev.cb = (bytes_to_copy_super_mem > MEM_WORDS_SUPERVISOR_COUNT? MEM_WORDS_SUPERVISOR_COUNT : bytes_to_copy_super_mem);
	real_machine -> ch_dev.st = HD_DISK;
	real_machine -> ch_dev.sb = ((file_entry -> offset / MEM_WORD_SIZE) / MEM_PAGE_SIZE);	// calculate the hard disk page
	real_machine -> ch_dev.of = file_entry -> offset % (MEM_WORD_SIZE * MEM_PAGE_SIZE);
	xchg(&real_machine -> ch_dev);
	bytes_to_copy_super_mem -= real_machine -> ch_dev.cb;
	
	// validate if the program starts with #LOS 
	// if so copy to user_mem
	real_machine_validate_supervisor(real_machine);
	
	for(uint32_t i = 0; i < MEM_SUPERVISOR_PAGE_COUNT; ++i) {
		uint16_t r_page = translate_to_real_address(&real_machine -> mem, user_mem_dest_page);
		real_machine -> ch_dev.dt = USER_MEM;
		real_machine -> ch_dev.db = r_page;
		real_machine -> ch_dev.cb = bytes_to_copy_super_mem > MEM_PAGE_SIZE? MEM_PAGE_SIZE : bytes_to_copy_user_mem;
		real_machine -> ch_dev.st = SHARED_MEM;
		real_machine -> ch_dev.sb = i * MEM_PAGE_SIZE;
		real_machine -> ch_dev.of = 0;
		xchg(&real_machine -> ch_dev);

		bytes_to_copy_user_mem -= real_machine -> ch_dev.cb;
		++user_mem_dest_page;		
	}


	// copy everything in between
	while(bytes_to_copy_super_mem > MEM_WORDS_SUPERVISOR_COUNT) {
		real_machine -> ch_dev.dt = SUPER_MEM;
		real_machine -> ch_dev.db = MEM_BEG_SUPERVISOR_ADDR;
		real_machine -> ch_dev.cb = (bytes_to_copy_super_mem > MEM_WORDS_SUPERVISOR_COUNT? MEM_WORDS_SUPERVISOR_COUNT : bytes_to_copy_super_mem);
		real_machine -> ch_dev.st = HD_DISK;
		real_machine -> ch_dev.sb = ((file_entry -> offset / MEM_WORD_SIZE) / MEM_PAGE_SIZE);	// calculate the hard disk page
		real_machine -> ch_dev.of = file_entry -> offset % (MEM_WORD_SIZE * MEM_PAGE_SIZE);	
		xchg(&real_machine -> ch_dev);
		bytes_to_copy_super_mem -= real_machine -> ch_dev.cb;

		// copy to user mem
	}

	// copy the last bytes if theres something left to copy
	if(bytes_to_copy_super_mem > 0) {

	}
	real_machine_validate_supervisor(real_machine);

	// validate the program
	
	// if the program is valid copy from super_mem to user_mem
	// start from 4 since the first word of the program is #LOS
	real_machine -> cpu.pc = CPU_DEFAULT_PC_VALUE;

	while(1) {
		virtual_machine_execute(&virtual_machine);

		if(real_machine -> cpu.pi + real_machine -> cpu.si > 0 || real_machine -> cpu.ti == 0) {
			interupt(&real_machine -> cpu);
		}	
	}

	destroy_virtual_machine(&virtual_machine);
}

int real_machine_validate_supervisor(Real_machine* real_machine) {
	return 0;
}