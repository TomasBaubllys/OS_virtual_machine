#ifndef HARD_DISK_H_INCLUDED
#define HARD_DISK_H_INCLUDED

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "file.h"
#include "util.h"

// [0x0000; 0x0003] - number of files
// [0x0004; 0x1000] - file entry array
// [0x1001; 0x7fff] - files 

#define HD_MAX_HARD_DISK_ADDRESS 0x8000
#define HD_FILE_ENTRY_START 0x0004
#define HD_FILES_START 0x1001
#define HD_HARD_DISK_FILE_NAME "__IMAGINE_DRAGONS.bin"
#define HD_OPERATION_MODE "ab+"
#define HD_OPEN_ERR "Hard disk could not be openned\n"
#define HD_CORUPTION_ERR "Hard disk was corrupted\n"

typedef struct Hard_disk {
	FILE *fptr;
} Hard_disk;

int read_write_arg_check_hard_disk(Hard_disk* hard_disk, uint16_t address);

int init_hard_disk(Hard_disk* hard_disk);

uint32_t read_word_hard_disk(Hard_disk* hard_disk, uint16_t address);

uint8_t* read_stream_hard_disk(Hard_disk* hard_disk, uint16_t address, uint16_t bytes_to_read);

uint32_t read_file_entries(Hard_disk* hard_disk, File_entry** files);

File_entry print_files(Hard_disk* hard_disk);

#endif // HARD_DISK_H_INCLUDED
