#ifndef FILE_H_INCLUDED
#define FILE_H_INCLUDED

#include <string.h>
#include <stdint.h>

#define FILE_NAME_SIZE 8
#define FILE_START_MARKER "#LOS"
#define FILE_END_MARKER "#BYE"
#define FILE_MARKER_SIZE 4
#define MAX_FILES HD_MAX_HARD_DISK_ADDRESS / (FILE_NAME_SIZE + FILE_MARKER_SIZE * 2)
#define MSG_DSPL_FILES "Files: \n"
#define MSG_SELECT_FILE "Select a file to load: \n"
#define FILE_TOO_BIG_ERR_MSG "File size exceeds maximum virtual machine memory\n"

typedef struct File_entry {
	char file_name[FILE_NAME_SIZE + 1];
	uint32_t offset;
	uint32_t size;
} File_entry;

#endif // FILE_H_INCLUDED
