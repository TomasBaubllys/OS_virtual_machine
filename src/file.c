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
		if(strcmp(marker, FILE_START_MARKER) == 0) {
			char raw_name[FILE_NAME_SIZE];
			if(fread(raw_name, 1, FILE_NAME_SIZE, hard_disk -> fptr) != FILE_NAME_SIZE) {
				break; // EOF
			}
	
			// save the file name
			memcpy(files[file_count].name, raw_name, FILE_NAME_SIZE);
			files[file_count].name[FILE_NAME_SIZE] = '\0';
			
			// save the beggininnig of the file
			files[file_count].offset = ftell(hard_disk -> fptr);
			
			// find BYE
			while(fread(marker, 1, FILE_MARKER_SIZE, hard_disk -> fptr) == FILE_MARKER_SIZE) {
				if(strcmp(marker, FILE_END_MARKER) == 0) {
					// calculate the file size
					files[file_count].size = files[file_count].offset - ftell(hard_disk -> fptr) - FILE_MARKER_SIZE;
					++file_count;
					
					if(file_count >= MAX_FILES) {
						return file_count;
					}
				}
				else {
					fseek(hard_disk -> fptr, -(FILE_MARKER_SIZE - 1), SEEK_CUR);
				}
			}
		}
	}
	
	return count;
}
