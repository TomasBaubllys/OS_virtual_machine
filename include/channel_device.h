#ifndef CHANNEL_DEVICE_H_INCLUDED
#define CHANNEL_DEVICE_H_INCLUDED

#include <stdint.h>
#include "hard_disk.h"

typedef struct Channel_device {
	uint16_t sb;			// number of the block to copy from
	uint16_t db;			// number of the block to copy to
	uint8_t st;				// number of the object which we are going to copy from
	uint32_t cb;			// number of bytes to copy (one byte is not used)
	uint16_t of;			// stores the offset value
	uint8_t dt;				// number of the object to copy to
	uint32_t sa;			// symbol register

	Memory* memory;			// bruh
	Hard_disk* Hard_disk	// bruh
} Channel_device;

enum Object_numbers {
	USER_MEM = 1,
	IO_STREAM,
	SHARED_MEM,
	RA_REG
};

int init_channel_device(Channel_device* channel_dev, Memory* memorys);

void reset_channel_device(Channel_device* channel_device);

int xchg(Channel_device* channel_device);

#endif // CHANNEL_DEVICE_H_INCLUDED
