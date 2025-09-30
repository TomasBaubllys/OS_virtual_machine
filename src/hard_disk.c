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
	
	/*if(size > HD_MAX_HARD_DISK_ADDRESS) {
		fprintf(stderr, HD_CORUPTION_ERR);
		fclose(hard_disk -> fptr);
		return -1;
	}*/
	
	fclose(hard_disk -> fptr);
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
	// check if word will fit
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

	if(address >= HD_MAX_HARD_DISK_ADDRESS - 4) {
		return 0;
	}

	if(fseek(hard_disk -> fptr, address, SEEK_SET) != 0) {
		perror("fseek");
		return 0;
	}

	uint32_t value = 0;

	if(fread(&value, sizeof(value), 1, hard_disk -> fptr) != 1) {
		perror("fread");
		return 0;
	}

	rewind(hard_disk -> fptr);
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
		perror("fseek");
		return 0;
	}

	uint8_t value = 0;
		
	if(fread(&value, sizeof(value), 1, hard_disk -> fptr) != 1) {
		perror("fread");
		return 0;
	}
	
	rewind(hard_disk -> fptr);

	return value;
}

uint8_t* read_stream_hard_disk(Hard_disk* hard_disk, uint16_t address, uint16_t bytes_to_read) {
	if(read_write_arg_check_hard_disk(hard_disk, address) != 0) {
		return NULL;
	}
	
	if(address + bytes_to_read >= HD_MAX_HARD_DISK_ADDRESS) {
		return NULL;
	}

	uint8_t* stream = malloc(bytes_to_read);

	if(!stream) {
		perror("malloc");
		return NULL;
	}
	
	if(fseek(hard_disk -> fptr, address, SEEK_SET) != 0) {
		perror("fseek");
		free(stream);
		return NULL;
	}

	if(fread(stream, sizeof(uint8_t), bytes_to_read, hard_disk -> fptr) != bytes_to_read) {
		free(stream);
		perror("fread");
		return NULL;
	}

	return stream;
}

uint32_t read_file_entries(Hard_disk* hard_disk, File_entry** files) {
	if(!hard_disk) {
		return 0;
	}

	// file must already exist
	hard_disk -> fptr = fopen(HD_HARD_DISK_FILE_NAME, "rb+");

	if(!hard_disk -> fptr) {
		fprintf(stderr, HD_OPEN_ERR);
		return 0;
	}

	uint32_t file_count = 0;
	if(fread(&file_count, sizeof(uint32_t), 1, hard_disk -> fptr) != 1) {
		fprintf(stderr, HD_CORUPTION_ERR);
		fclose(hard_disk -> fptr);
		return 0;
	}

	*files = malloc(sizeof(File_entry) * file_count);	

	if(!*files) {
		perror("malloc: ");
		fclose(hard_disk -> fptr);
		return 0;
	}

	for(uint32_t i = 0; i < file_count; ++i) {
		if(fread((*files)[i].file_name, sizeof((*files)[i].file_name - 1), 1, hard_disk -> fptr) != 1) {
			fprintf(stderr, HD_CORUPTION_ERR);
			fclose(hard_disk -> fptr);
			free(*files);
			*files = NULL;
			return 0;
		}
		(*files)[i].file_name[FILE_NAME_SIZE] = '\0';

		if(fread(&((*files)[i].offset), sizeof(uint32_t), 1, hard_disk -> fptr) != 1) {
			fprintf(stderr, HD_CORUPTION_ERR);
			fclose(hard_disk -> fptr);
			free(*files);
			*files = NULL;
			return 0;
		}
		if(fread(&((*files)[i].size), sizeof(uint32_t), 1, hard_disk -> fptr) != 1) {
			fprintf(stderr, HD_CORUPTION_ERR);
			fclose(hard_disk -> fptr);
			free(*files);
			*files = NULL;
			return 0;
		}
	}

	fclose(hard_disk -> fptr);

	return file_count;
}