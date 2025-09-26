#ifndef FILE_H_INCLUDED
#define FILE_H_INCLUDED

#include "hard_disk.h"
#include "string.h"

#define FILE_NAME_SIZE 8
#define FILE_START_MARKER "#LOS"
#define FILE_END_MARKER "#BYE"
#define FILE_MARKER_SIZE 4
#define MAX_FILES HD_MAX_HARD_DISK_ADDRESS / (FILE_NAME_SIZE + FILE_MARKER_SIZE * 2)
#define MSG_DSPL_FILES "Files: \n"
#define MSG_SELECT_FILE "Select a file to load: \n"

typedef struct File_entry {
	char file_name[FILE_NAME_SIZE + 1];
	uint32_t offset;
	uint32_t size;
} File_entry;

// reads all the available files returns the amount of files found
uint32_t read_file_entries(Hard_disk* hard_disk, File_entry* files);

File_entry print_files(Hard_disk* Hard_disk);

// please dont forget to free the value after using it
uint32_t* read_program(Hard_disk* hard_disk, File_entry* file_entry, uint32_t* return_size);

#endif // FILE_H_INCLUDED
