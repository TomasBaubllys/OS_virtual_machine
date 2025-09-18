#include "../include/memory.h"

int init_memory(Memory* mem) {
	if(!mem) {
		return -1;
	}	

	memset(mem -> user, 0, sizeof(mem -> user));
	memset(mem -> shared, 0, sizeof(mem -> share));
	memset(mem -> supervisor, 0, sizeof(mem -> supervisor));
	
	return 0;
}
