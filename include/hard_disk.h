#ifndef HARD_DISK_H_INCLUDED
#define HARD_DISK_H_INCLUDED

#include <stdint.h>
#include <stdio.h>

#define HD_MAX_HARD_DISK_ADDRESS 0x2000
#define HD_HARD_DISK_FILE_NAME "__IMAGINE_DRAGONS.bin"
#define HD_OPERATION_MODE "ab+"

typedef struct Hard_disk {
	FILE *fptr;
} Hard_disk;

int init_hard_disk(Hard_disk* hard_disk);

#endif // HARD_DISK_H_INCLUDED
