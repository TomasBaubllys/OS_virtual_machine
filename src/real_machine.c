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

int add_virtual_machine(Real_machine* real_machine) {
	if(!real_machine) {
		return -1;
	} 

	Virtual_machine* vm = calloc(1, sizeof(Virtual_machine));
	if(!vm) {
		return -1;
	}

	if(init_virtual_machine(vm, &real_machine -> cpu, &real_machine -> mem) != 0) {
		free(vm);
		return -1;
	}

	real_machine -> vm = vm;

	return 0;
}

void remove_virtual_machine(Real_machine* real_machine) {
	if(!real_machine) {
		return;
	}
	
	if(!real_machine -> vm) {
		return;
	}

	destroy_virtual_machine(real_machine -> vm);
	free(real_machine -> vm);
	real_machine -> vm = NULL;
}

int destroy_real_machine(Real_machine* real_machine) {
	if(!real_machine) {
		return -1;
	}	

	if(real_machine -> vm) {
		destroy_virtual_machine(real_machine -> vm);
		free(real_machine -> vm);
		real_machine -> vm = NULL;
	}

	return 0;
}

void real_machine_run(Real_machine* real_machine, File_entry* file_entry) {
	if(!real_machine || !file_entry) {
		return;
	}

	if(file_entry -> size >= MEM_PAGE_SIZE * MEM_WORD_SIZE * MEM_SUPERVISOR_PAGE_COUNT) {
		fprintf(stderr, FILE_TOO_BIG_ERR_MSG);
		return;
	}

	if(add_virtual_machine(real_machine) != 0) {
		fprintf(stderr, RM_MSG_NULL_VM);
		return;
	}

	load_program_supervisor(real_machine, file_entry);

	if(real_machine_validate_supervisor(real_machine, file_entry -> size) != 0) {
		fprintf(stderr, RM_MSG_BAD_PROGRAM);
		remove_virtual_machine(real_machine);
		return;
	}

	// if program was valid copy it except #LOS and #BYE
	load_program_user(real_machine, file_entry -> size - (MEM_WORD_SIZE * 2));

	real_machine -> cpu.pc = CPU_DEFAULT_PC_VALUE;

	while(1) {
		virtual_machine_execute(real_machine -> vm);

		if(real_machine -> cpu.pi + real_machine -> cpu.si > 0 || real_machine -> cpu.ti == 0) {
			if(interrupt(&real_machine -> cpu) == INTERUPT_STOP) {
				break;
			}
		}	
	}

	remove_virtual_machine(real_machine);
}

int real_machine_validate_supervisor(Real_machine* real_machine, uint32_t expected_program_length) {
	uint32_t los_count = 0;
	uint32_t offset = 0;

	while(offset < MEM_WORDS_SUPERVISOR_COUNT * MEM_WORD_SIZE) {
		uint32_t word = read_word(&real_machine -> mem, MEM_BEG_SUPERVISOR_ADDR + offset);

		if(word == MEM_PROGRAM_LOS_UINT32) {
			++los_count;
		}
		else if(word == MEM_PROGRAM_BYE_UINT32) {
			offset += MEM_WORD_SIZE;
			break;
		}

		offset += MEM_WORD_SIZE;
	}

	if(los_count > 1 || offset != expected_program_length) {
		return -1;
	}

	return 0;
}

int load_program_supervisor(Real_machine* real_machine, File_entry* file_entry) {
	real_machine -> ch_dev.dt = SUPER_MEM;
	real_machine -> ch_dev.db = MEM_SUPERVISOR_PAGE_BEGIN;
	real_machine -> ch_dev.cb = (file_entry -> size < MEM_WORDS_SUPERVISOR_COUNT * MEM_WORD_SIZE? file_entry -> size : MEM_WORD_SIZE * MEM_WORDS_SUPERVISOR_COUNT);
	real_machine -> ch_dev.st = HD_DISK;
	real_machine -> ch_dev.sb = ((file_entry -> offset / MEM_WORD_SIZE) / MEM_PAGE_SIZE);	// calculate the hard disk page
	real_machine -> ch_dev.of = file_entry -> offset % (MEM_PAGE_BYTE_COUNT);
	return xchg(&real_machine -> ch_dev);
}

int load_program_user(Real_machine* real_machine, uint32_t program_length) {
	for(uint32_t i = 0; i < MEM_SUPERVISOR_PAGE_COUNT; ++i) {
		uint16_t r_page = translate_to_real_address(&real_machine -> mem, i * MEM_PAGE_BYTE_COUNT) / (MEM_PAGE_BYTE_COUNT);
		real_machine -> ch_dev.dt = USER_MEM;
		real_machine -> ch_dev.db = r_page;
		real_machine -> ch_dev.cb = program_length > MEM_PAGE_BYTE_COUNT? MEM_PAGE_BYTE_COUNT : program_length;
		real_machine -> ch_dev.st = SUPER_MEM;
		real_machine -> ch_dev.sb = MEM_SUPERVISOR_PAGE_BEGIN + i;
		real_machine -> ch_dev.of = 0;
		real_machine -> ch_dev.sa = (i == 0? 4 : 0);
		xchg(&real_machine -> ch_dev);

		program_length -= real_machine -> ch_dev.cb;		

		if(program_length == 0) {
			break;
		}
	}

	return 0;
}
