#include "../include/channel_device.h"

int init_channel_device(Channel_device* channel_device) {
	if(!channel_device) {
		return -1;
	}
	
	channel_device -> sb = 0;
	channel_device -> db = 0;
	channel_device -> st = 0;
	channel_device -> cb = 0;
	channel_device -> of = 0;
	channel_device -> dt = 0;

	return 0;
}
