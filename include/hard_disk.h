#ifndef HARD_DISK_H_INCLUDED
#define HARD_DISK_H_INCLUDED

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define HD_MAX_HARD_DISK_ADDRESS 0x2000
#define HD_HARD_DISK_FILE_NAME "__IMAGINE_DRAGONS.bin"
#define HD_OPERATION_MODE "ab+"
#define HD_CORRUPTION_ERR "Hard disk was corrupted"

typedef struct Hard_disk {
	FILE *fptr;
} Hard_disk;

int init_hard_disk(Hard_disk* hard_disk);

int write_byte_hard_disk(Hard_disk* hard_disk, uint16_t address, uint8_t value);

int write_word_hard_disk(Hard_disk* hard_disk, uint16_t address, uint32_t value);

#endif // HARD_DISK_H_INCLUDED
