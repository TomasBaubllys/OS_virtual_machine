include "../include/cpu.h"

int init_cpu(CPU* cpu) {
	if(!cpu) {
		return -1;
	}
	
	cpu -> pc = 0;
	cpu -> pi = 0;
	cpu -> si = 0;
	cpu -> tr = 0;
	cpu -> ti = _CPU_DEFAULT_TIMER_VALUE;
	cpu -> sf = 0;
	cpu -> mr = _CPU_USER_MODE;
	cpu -> ss = 0;
	cpu -> ra = 0;
	cpu -> rb = 0;
	cpu -> rc = 0;
	
	return 0;
}
