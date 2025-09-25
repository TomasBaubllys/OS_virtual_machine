#include "../include/file.h"


uint32_t read_file_entries(Hard_disk* hard_disk, File_entry* files) {
	if(!hard_disk || !files) {
		return 0;
	}

	if(!hard_disk -> fptr) {
		return 0;
	}

	// rewind the hard_disk pointer to the beggining of the file
	rewind(hard_disk -> fptr);

	uint32_t file_count = 0;
	char marker[FILE_MARKER_SIZE + 1] = {0};

	while(fread(marker, 1, FILE_MARKER_SIZE, hard_disk -> fptr) == FILE_MARKER_SIZE) {
		// look for file beggining marker
		if(memcmp(marker, FILE_START_MARKER, FILE_MARKER_SIZE) == 0) {
			char raw_name[FILE_NAME_SIZE];
			if(fread(raw_name, 1, FILE_NAME_SIZE, hard_disk -> fptr) != FILE_NAME_SIZE) {
				break; // EOF
			}

			// save the file name
			memcpy(files[file_count].file_name, raw_name, FILE_NAME_SIZE);
			files[file_count].file_name[FILE_NAME_SIZE] = '\0';
			
			// save the beggininnig of the file
			files[file_count].offset = ftell(hard_disk -> fptr);
			
			// find BYE
			while(fread(marker, 1, FILE_MARKER_SIZE, hard_disk -> fptr) == FILE_MARKER_SIZE) {
				if(memcmp(marker, FILE_END_MARKER, FILE_MARKER_SIZE) == 0) {
					// calculate the file size
					files[file_count].size = ftell(hard_disk -> fptr) - FILE_MARKER_SIZE - files[file_count].offset;
					++file_count;
					
					if(file_count >= MAX_FILES) {
						return file_count;
					}
					break;
				}
				else {
					fseek(hard_disk -> fptr, -(FILE_MARKER_SIZE - 1), SEEK_CUR);
				}
			}
		}
		else {
			fseek(hard_disk -> fptr, -(FILE_MARKER_SIZE - 1), SEEK_CUR);
		}
	}
	
	return file_count;
}

File_entry print_files(Hard_disk* hard_disk){
	File_entry files[MAX_FILES];
	uint32_t file_count = read_file_entries(hard_disk, files);
	int choice  = 0;


	do{
		printf("%s", MSG_DSPL_FILES);
		for(uint32_t i = 0; i < file_count; ++i) {
			printf("%d. name: %s, offset: %x, size: %x\n", i + 1, files[i].file_name, files[i].offset, files[i].size);
		}

		printf("%s", MSG_SELECT_FILE);

		if (scanf("%d", &choice) != 1) {
			while (getchar() != '\n'); 
			choice = 0;
		}
	} while(choice < 1 || choice > file_count);

	return files[choice - 1];

}

uint32_t* read_program(Hard_disk* hard_disk, File_entry* file_entry, uint32_t* return_size) {
	if(!hard_disk || !file_entry || !return_size) {
		return NULL;
	}
	
	if(!(hard_disk -> fptr)) {
		return NULL;
	}

	if((file_entry -> size % MEM_WORD_SIZE) != 0) {
		return NULL;
	}

	fseek(hard_disk -> fptr, file_entry -> offset, SEEK_SET);
	*return_size = (file_entry -> size) / MEM_WORD_SIZE;
	
	uint32_t* program = malloc(MEM_WORD_SIZE * (*return_size)); 
	if(!program) {
		return NULL;
	}

	if(fread(program, MEM_WORD_SIZE, *return_size, hard_disk -> fptr) != *return_size) {
		free(program);
		return NULL;
	}

	return program;
}
