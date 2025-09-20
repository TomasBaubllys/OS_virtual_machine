#include "../hard_disk.h"

int init_hard_disk(Hard_disk* hard_disk) {
	if(!hard_disk) {
		return -1;
	}

	hard_disk -> fptr = fopen(); // lalalala	
}
