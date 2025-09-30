#include "../include/hard_disk.h"
#include <assert.h>

int main(int argc, char* argv[]) {
	Hard_disk hd;
	init_hard_disk(&hd);

	File_entry* flarr;

	int size = read_file_entries(&hd, &flarr);

	for(int i = 0; i < size; ++i) {
		printf("NAME : %s, OFFSET: %x, SIZE: %x\n", flarr[i].file_name, flarr[i].offset, flarr[i].size);
	}

	return 0;
}
