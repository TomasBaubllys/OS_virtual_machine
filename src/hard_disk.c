#include "../include/hard_disk.h"

int read_write_arg_check_hard_disk(Hard_disk* hard_disk, uint16_t address) {
	if(!hard_disk) {
		return -1;
	}

	if(!hard_disk -> fptr) {
		return -1;
	}

	if(address >= HD_MAX_HARD_DISK_ADDRESS) {
		return -1;
	}

	return 0;
}

int init_hard_disk(Hard_disk* hard_disk) {
	if(!hard_disk) {
		return -1;
	}

	hard_disk -> fptr = fopen(HD_HARD_DISK_FILE_NAME, "rb+");
	
	if(!(hard_disk -> fptr)) {
		hard_disk -> fptr = fopen(HD_HARD_DISK_FILE_NAME, HD_OPERATION_MODE);
	}

	if(!(hard_disk -> fptr)) {
		return -1;
	}
	
	// read the hard drive size
	fseek(hard_disk -> fptr, 0l, SEEK_END);
	long size = ftell(hard_disk -> fptr);
	rewind(hard_disk -> fptr);	

	if(size == 0) {
		uint8_t* zero = calloc(HD_MAX_HARD_DISK_ADDRESS, 1);
		if(!zero) {
			fclose(hard_disk -> fptr);
			return -1;
		}

		if(fwrite(zero, 1, HD_MAX_HARD_DISK_ADDRESS, hard_disk -> fptr) != HD_MAX_HARD_DISK_ADDRESS) {
			fclose(hard_disk -> fptr);
			free(zero);
			return -1;
		} 
	
		free(zero);
	}		
	else if(size != HD_MAX_HARD_DISK_ADDRESS) {
		fprintf(stderr, HD_CORRUPTION_ERR);
		fclose(hard_disk -> fptr);
		return -1;
	}

	return 0;
}

int write_byte_hard_disk(Hard_disk* hard_disk, uint16_t address, uint8_t value) {
	if(read_write_arg_check_hard_disk(hard_disk, address) != 0) {
		return -1;
	}

	if(fseek(hard_disk -> fptr, address, SEEK_SET) != 0) {
		perror("fseek");
		return -1;
	}

	if(fwrite(&value, sizeof(value), 1, hard_disk -> fptr) != 1) {
		perror("fwrite");
	}
	
	rewind(hard_disk -> fptr);
	return 0;
}

int write_word_hard_disk(Hard_disk* hard_disk, uint16_t address, uint32_t value) {
	if(read_write_arg_check_hard_disk(hard_disk, address) != 0) {
		return -1;
	}
	
	if(fseek(hard_disk -> fptr, address, SEEK_SET) != 0) {
		perror("fseek");
		return -1;
	}

	if(fwrite(&value, sizeof(value), 1, hard_disk -> fptr) != 1) {
		perror("fwrite");
	}
	
	rewind(hard_disk -> fptr);
	return 0;
}

uint32_t read_word_hard_disk(Hard_disk* hard_disk, uint16_t address) {
	if(read_write_arg_check_hard_disk(hard_disk, address) != 0) {
		return 0;
	}

	if(address >= HD_MAX_HARD_DISK_ADDRESS - MEM_WORD_SIZE) {
		return 0;
	}

	if(fseek(hard_disk -> fptr, address, SEEK_SET) != 0) {
		return 0;
	}

	uint32_t value = 0;

	if(fread(&value, sizeof(value), 1, hard_disk -> fptr) != 1) {
		return 0;
	}

	return value;
}

uint8_t read_byte_hard_disk(Hard_disk* hard_disk, uint16_t address) {
	if(read_write_arg_check_hard_disk(hard_disk, address) != 0) {
		return 0;
	}

	if(address >= HD_MAX_HARD_DISK_ADDRESS - 1) {
		return 0;
	}
	
	if(fseek(hard_disk -> fptr, address, SEEK_SET) != 0) {
		return 0;
	}

	uint8_t value = 0;
		
	if(fread(&value, sizeof(value), 1, hard_disk -> fptr) != 1) {
		return 0;
	}

	return value;
}
