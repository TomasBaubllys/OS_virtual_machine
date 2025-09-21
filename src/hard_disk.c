#include "../include/hard_disk.h"

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
