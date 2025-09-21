#ifndef CHANNEL_DEVICE_H_INCLUDED
#define CHANNEL_DEVICE_H_INCLUDED

#include <stdint.h>

typedef struct Channel_device {
	uint16_t sb;			// number of the block to copy from
	uint16_t db;			// number of the block to copy to
	
	uint8_t st;			// number of the object which we are going to copy from

	uint32_t cb;			// number of bytes to copy (one byte is not used)
	uint16_t of;			// stores the offset value
	
	uint8_t dt;			// number of the object to copy to

} Channel_device;

int init_channel_device(Channel_device* channel_dev);

#endif // CHANNEL_DEVICE_H_INCLUDED
