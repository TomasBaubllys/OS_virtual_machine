#include "../include/real_machine.h"
#include "../include/virtual_machine.h"

int init_real_machine(Real_machine* real_machine) {
	if(!real_machine) {
		return -1;
	}

	if(init_cpu(&(real_machine -> cpu)) != 0) {
		return -1;
	}

	if(init_memory(&(real_machine -> mem)) != 0) {
		return -1;
	}

	if(init_hard_disk(&(real_machine -> hd)) != 0) {
		return -1;
	}

	if(init_channel_device(&(real_machine -> ch_dev)) != 0) {
		return -1;
	}
	
	// allocate space for RM_VM_MAX_COUNT virtual machines
	real_machine -> vm = malloc(sizeof(Virtual_machine) * RM_VM_MAX_COUNT);
		
	real_machine -> vm_count = 0;
	
	return 0;
};

int execute_command(Real_machine* real_machine, uint8_t virtual_machine_index, uint32_t command) {
	uint16_t com_code = command >> 16;

	if(virtual_machine_index >= real_machine -> vm_count) {
		return -1;
	}

	switch(com_code) {
		// memory commands
		// MOxy
		case 0x4d4f: {
			uint8_t x = char_hex_to_decimal((command & 0x0000ff00) >> 8);
			uint8_t y = char_hex_to_decimal(command & 0x000000ff);
	
			if(x > 0xf || y > 0xf || x * 16 + y >= MEM_MAX_USER_VM_ADDRESS) {
				real_machine -> cpu.pi = RM_PI_INVALID_ADDRESS;
				break;
			}
			
			real_machine -> cpu.ra = x * 16 + y;
			real_machine -> vm[virtual_machine_index].pc += MEM_WORD_SIZE;
			break;
		}
		// LRa 
		// copy RA value to a value
		case 0x4c52: {	
			uint8_t valid = 1;
			uint16_t reg = command & 0x0000ffff;
			uint8_t reg_nr = get_register_num(reg);
			if(reg_nr == CPU_UNKNOWN_REGISTER) {
				real_machine -> cpu.pi = RM_PI_INVALID_ASSIGNMENT;
				break;
			}

			switch (reg_nr){
				case RA:
					// nothing
					break;
				case RB:
					real_machine -> cpu.rb = real_machine -> cpu.ra;
					break;
				case RC:
					real_machine -> cpu.rc = real_machine -> cpu.ra;
					break;
				default:
					real_machine -> cpu.pi = RM_PI_INVALID_ASSIGNMENT;
					valid = 0;
					break;
			}			
			
			if(valid){
				real_machine -> vm[virtual_machine_index].pc += MEM_WORD_SIZE;	
			}

			break;
		}
		// LLa
		// copy a value to RB value
		case 0x4c4c:{
			uint8_t valid = 1;
			uint16_t reg = command & 0x0000ffff;
			uint8_t reg_nr = get_register_num(reg);
			if(reg_nr == CPU_UNKNOWN_REGISTER){
				real_machine -> cpu.pi = RM_PI_INVALID_ASSIGNMENT;
				break;
			}

			switch (reg_nr){
				case RA:
					real_machine -> cpu.rb = real_machine -> cpu.ra;
					break;
				case RB:
					// nothing
					break;
				case RC:
					real_machine -> cpu.rb = real_machine -> cpu.rc;
					break;
				default:
					real_machine -> cpu.pi = RM_PI_INVALID_ASSIGNMENT;
					valid = 0;
					break;	 	
				}
			if(valid){
				real_machine -> vm[virtual_machine_index].pc += MEM_WORD_SIZE;
			}
			break;
		}
		// LWxy
		// value at address x*16 + y copying to RA
		case 0x4c57: {
			uint8_t x = char_hex_to_decimal((command & 0x0000ff00) >> 8);
			uint8_t y = char_hex_to_decimal(command & 0x000000ff);
			
			if(x > 0xf || y > 0xf || x * 16 + y >= MEM_MAX_USER_VM_ADDRESS) {
				real_machine -> cpu.pi = RM_PI_INVALID_ADDRESS;
				break;
			}
			
			uint16_t vm_addr = x * 16 + y;

			uint16_t r_addr = translate_to_real_address(real_machine, vm_addr, real_machine -> vm[virtual_machine_index].page_table_index);
			real_machine -> ch_dev.dt = RA_REG;  
			real_machine -> ch_dev.st = USER_MEM;
			
			real_machine -> ch_dev.of = r_addr;
			real_machine -> cpu.si = RM_SI_LW;
			real_machine -> vm[virtual_machine_index].pc += MEM_WORD_SIZE;
			break;
		}
		// SWxy
		case 0x5357: {
			uint8_t x = char_hex_to_decimal((command & 0x0000ff00) >> 8);
			uint8_t y = char_hex_to_decimal(command & 0x000000ff);
			
			if(x > 0xf || y > 0xf || x * 16 + y >= MEM_MAX_USER_VM_ADDRESS) {
				real_machine -> cpu.pi = RM_PI_INVALID_ADDRESS;
				break;
			}
			
			uint16_t vm_addr = x * 16 + y;

			uint16_t r_addr = translate_to_real_address(real_machine, vm_addr, real_machine -> vm[virtual_machine_index].page_table_index);
			real_machine -> ch_dev.dt = USER_MEM;
			real_machine -> ch_dev.st = RA_REG;	
	
			real_machine -> ch_dev.of = r_addr;
			real_machine -> cpu.si = RM_SI_SW;
			real_machine -> vm[virtual_machine_index].pc += MEM_WORD_SIZE;
			break;
		}
		// BP
		case 0x4250: {
			if(real_machine -> cpu.ss == 1){
				break;
			}
			else{
				real_machine -> cpu.ss = 1;
			}
			uint8_t x = char_hex_to_decimal((command & 0x0000ff00) >> 8);
			uint8_t y = char_hex_to_decimal(command & 0x000000ff);
			
			if(x > 0xf || y > 0xf || x * 16 + y > MEM_MAX_SHARED_ADDRESS) {
				real_machine -> cpu.pi = RM_PI_INVALID_ADDRESS;
				break;
			}
			
			real_machine -> ch_dev.of = x * 16 + y;

			real_machine -> ch_dev.st = RA_REG;
			real_machine -> ch_dev.dt = SHARED_MEM;
			real_machine -> cpu.si = RM_SI_BP;
			real_machine -> vm[virtual_machine_index].pc += MEM_WORD_SIZE;
			break;
		}
		// BG
		case 0x4247: {		
			if(real_machine -> cpu.ss == 1){
				break;
			}
			else{
				real_machine -> cpu.ss = 1;
			}
			uint8_t x = char_hex_to_decimal((command & 0x0000ff00) >> 8);
			uint8_t y = char_hex_to_decimal(command & 0x000000ff);
			
			if(x > 0xf || y > 0xf || x * 16 + y > MEM_MAX_SHARED_ADDRESS) {
				real_machine -> cpu.pi = RM_PI_INVALID_ADDRESS;
				break;
			}
			
			real_machine -> ch_dev.of = x * 16 + y;

			real_machine -> ch_dev.st = SHARED_MEM;
			real_machine -> ch_dev.dt = RA_REG;
			real_machine -> cpu.si = RM_SI_BP;
			real_machine -> vm[virtual_machine_index].pc += MEM_WORD_SIZE;
			break;
		}
		
		// HD
		case 0x4844: {
			if(real_machine -> cpu.mr != CPU_SUPERVISOR_MODE){
				break;
			}

			uint8_t x = char_hex_to_decimal((command & 0x0000ff00) >> 8);
			uint8_t y = char_hex_to_decimal(command & 0x000000ff);
			
			if(x > 0xf || y > 0xf || x * 16 + y > MEM_MAX_USER_VM_ADDRESS) {
				real_machine -> cpu.pi = RM_PI_INVALID_ADDRESS;
				break;
			}
			
			real_machine -> ch_dev.of = x * 16 + y;
			
			// real_machine -> ch_dev.of = translate_to_real_address(real_machine, v_addr, real_machine -> vm[virtual_machine_index].page_table_index);
			
			real_machine -> ch_dev.of = x * 16 + y;
			real_machine -> ch_dev.cb = real_machine -> cpu.rc;

			real_machine -> ch_dev.dt = USER_MEM;
			real_machine -> ch_dev.st = DISK_MEM;
			real_machine -> cpu.si = RM_SI_BP;
			real_machine -> vm[virtual_machine_index].pc += MEM_WORD_SIZE; 
			break;
		}	
	
		// HR
		case 0x4852: {
			if(real_machine -> cpu.mr != CPU_SUPERVISOR_MODE){
				break;
			}
			
			uint8_t x = char_hex_to_decimal((command & 0x0000ff00) >> 8);
			uint8_t y = char_hex_to_decimal(command & 0x000000ff);
			
			if(x > 0xf || y > 0xf || x * 16 + y > MEM_MAX_SHARED_ADDRESS) {
				real_machine -> cpu.pi = RM_PI_INVALID_ADDRESS;
				break;
			}
			
			real_machine -> ch_dev.of = x * 16 + y;
			
			// real_machine -> ch_dev.of = translate_to_real_address(real_machine, v_addr, real_machine -> vm[virtual_machine_index].page_table_index);
			real_machine -> ch_dev.dt = DISK_MEM;
			real_machine -> ch_dev.st = USER_MEM;
			real_machine -> ch_dev.cb = real_machine -> cpu.rc;
			real_machine -> cpu.si = RM_SI_BP;
			real_machine -> vm[virtual_machine_index].pc += MEM_WORD_SIZE;
			break;
		}
	
		// I/O commands
		// GEDA
		case 0x4745: {			
			uint16_t rest_com = command & 0x0000ffff;
			if(rest_com != 0x4441) {
				real_machine -> cpu.pi = RM_PI_INVALID_OPCODE;
				break;
			}
			
			real_machine -> ch_dev.dt = RA_REG;
			real_machine -> ch_dev.st = IO_STREAM;
			real_machine -> cpu.si = RM_SI_GEDA;
			real_machine -> vm[virtual_machine_index].pc += MEM_WORD_SIZE;
			break;		
		}

		// PUTA
		case 0x5055: {
			uint16_t rest_com = command & 0x0000ffff;
			if(rest_com != 0x5441) {
				real_machine -> cpu.pi = RM_PI_INVALID_OPCODE;
				break;
			}
		
			real_machine -> ch_dev.dt = IO_STREAM;
			real_machine -> ch_dev.st = RA_REG;
			real_machine -> cpu.si = RM_SI_PUTA;
			real_machine -> vm[virtual_machine_index].pc += MEM_WORD_SIZE;
			break;
		}
		
		// PSTR
		case 0x5053: {
			uint16_t rest_com = command & 0x0000ffff;
			if(rest_com != 0x5452) {
				real_machine -> cpu.pi = RM_PI_INVALID_OPCODE;
				break;
			}
			
			if(real_machine -> cpu.ra >= MEM_MAX_USER_VM_ADDRESS) {
				real_machine -> cpu.pi = RM_PI_INVALID_ADDRESS;
				break;
			}
			
			// translate ra address to a real address
			// real_machine -> cpu.ra = translate_to_real_address(real_machine, real_machine -> cpu.ra, real_machine -> vm[virtual_machine_index].page_table_index);			

			real_machine -> ch_dev.dt = IO_STREAM;
			real_machine -> ch_dev.st = USER_MEM;
			real_machine -> ch_dev.cb = real_machine -> cpu.rc;
			real_machine -> cpu.si = RM_SI_PSTR;
			real_machine -> vm[virtual_machine_index].pc += MEM_WORD_SIZE;
			break;
		}

		// aritmetic commands
		// APxy	
		case 0x4150: {
			uint8_t x = char_hex_to_decimal((command & 0x0000ff00) >> 8);
			uint8_t y = char_hex_to_decimal(command & 0x000000ff);
			if(x > 0xf || y > 0xf || x * 16 + y >= MEM_MAX_USER_VM_ADDRESS) {
				real_machine -> cpu.pi = RM_PI_INVALID_ADDRESS;
				break;
			}
			
			real_machine -> cpu.ra += x * 16 + y;
			real_machine -> vm[virtual_machine_index].pc += MEM_WORD_SIZE;
			break;
		}
		// ADa
		// add a value to RA 
		case 0x4144:{
			uint8_t valid = 1;
			uint16_t reg = command & 0x0000ffff;
			uint8_t reg_nr = get_register_num(reg);
			if(reg_nr == CPU_UNKNOWN_REGISTER) {
				real_machine -> cpu.pi = RM_PI_INVALID_ASSIGNMENT;
				break;
			}

			switch (reg_nr){
				case RA:
					real_machine -> cpu.ra += real_machine -> cpu.ra; 
					break;
				case RB:
					real_machine -> cpu.ra += real_machine -> cpu.rb;
					break;
				case RC:
					real_machine -> cpu.ra += real_machine -> cpu.rc;
					break;
				default:
					real_machine -> cpu.pi = RM_PI_INVALID_ASSIGNMENT;
					valid = 0;
					break;
			}			
			
			if(valid){
				real_machine -> vm[virtual_machine_index].pc += MEM_WORD_SIZE;
			}
			break;
		}
		// SBa
		// substract a value to RA 
		case 0x5342:{
			uint8_t valid = 1;
			uint16_t reg = command & 0x0000ffff;
			uint8_t reg_nr = get_register_num(reg);
			if(reg_nr == CPU_UNKNOWN_REGISTER) {
					real_machine -> cpu.pi = RM_PI_INVALID_ASSIGNMENT;
					break;			
			}

			switch (reg_nr){
				case RA:
					real_machine -> cpu.ra -= real_machine -> cpu.ra; 
					break;
				case RB:
					real_machine -> cpu.ra -= real_machine -> cpu.rb;
					break;
				case RC:
					real_machine -> cpu.ra -= real_machine -> cpu.rc;
					break;
				default:
					// other registers not allowed
					real_machine -> cpu.pi = RM_PI_INVALID_ASSIGNMENT;
					valid = 0;
					break;			
				}			
				
			if(valid){
				real_machine -> vm[virtual_machine_index].pc += MEM_WORD_SIZE;
			}
			break;
		}
		// MUa
		// multiplies a value by RA value 
		// carry flag
		// sf 0000011
		case 0x4D55:{
			uint8_t valid = 1;
			uint16_t reg = command & 0x0000ffff;
			uint8_t reg_nr = get_register_num(reg);
			if(reg_nr == CPU_UNKNOWN_REGISTER) {
					real_machine -> cpu.pi = RM_PI_INVALID_ASSIGNMENT;
					break;
			}

			switch (reg_nr){
				case RA:
					real_machine -> cpu.ra *= real_machine -> cpu.ra; 
					break;
				case RB:
					real_machine -> cpu.ra *= real_machine -> cpu.rb;
					break;
				case RC:
					real_machine -> cpu.ra *= real_machine -> cpu.rc;
					break;
				default:
					// other registers not allowed
					real_machine -> cpu.pi = RM_PI_INVALID_ASSIGNMENT;
					valid = 0;
					break;			
				}			
			if(valid){
				real_machine -> vm[virtual_machine_index].pc += MEM_WORD_SIZE;
			}
			break;
		}
		// DVa
		// dividexs RA value by a value 
		case 0x4456:{
			uint8_t valid = 1;
			uint16_t reg = command & 0x0000ffff;
			uint8_t reg_nr = get_register_num(reg);
			if(reg_nr == CPU_UNKNOWN_REGISTER) {
					real_machine -> cpu.pi = RM_PI_INVALID_ASSIGNMENT;
					break;
			}

			switch (reg_nr){
				case RA:
					if(real_machine -> cpu.ra == 0){
						real_machine -> cpu.pi = RM_PI_DIVISION_BY_ZERO;
						valid = 0;
						break;
					}
					real_machine -> cpu.ra /= real_machine -> cpu.ra; 
					
					break;
				case RB:
					if(real_machine -> cpu.rb == 0){
						real_machine -> cpu.pi = RM_PI_DIVISION_BY_ZERO;
						valid = 0;
						break;
					}
					uint32_t temp = real_machine -> cpu.ra;
					real_machine -> cpu.ra /= real_machine -> cpu.rb;
					real_machine -> cpu.rb  = temp % real_machine -> cpu.rb;
					break;
				case RC:
					if(real_machine -> cpu.rc == 0){
						real_machine -> cpu.pi = RM_PI_DIVISION_BY_ZERO;
						valid = 0;
						break;
					}
					uint32_t temp1 = real_machine -> cpu.ra;
					real_machine -> cpu.ra /= real_machine -> cpu.rc;
					real_machine -> cpu.rc  = temp1 % real_machine -> cpu.rc;
					break;
				default:
					// other registers not allowed
					real_machine -> cpu.pi = RM_PI_INVALID_ASSIGNMENT;
					valid = 0;
					break;			
				}	
			if(valid){
				real_machine -> vm[virtual_machine_index].pc += MEM_WORD_SIZE;
			}		
			break;
		}
		// CMxy
		// (x*16 + y == ra)
		case 0x434d:{
			uint8_t x = char_hex_to_decimal((command & 0x0000ff00) >> 8);
			uint8_t y = char_hex_to_decimal(command & 0x000000ff);
			if(x > 0xf || y > 0xf || x * 16 + y >= MEM_MAX_USER_VM_ADDRESS) {
				real_machine -> cpu.pi = RM_PI_INVALID_ADDRESS;
				break;
			}

			// ZF = 1 CF = 0
			if(x * 16 + y == real_machine -> cpu.ra){
				real_machine -> cpu.sf ^= 0x0002; // 0000 0000 0000 0010
				real_machine -> cpu.sf &= 0xfffe; // 1111 1111 1111 1110
			}
			// ZF = 0 CF = 1
			else if(x * 16 + y < real_machine -> cpu.ra){
				real_machine -> cpu.sf &=  0xfffd; // 1111 1111 1111 1101
				real_machine -> cpu.sf ^= 0x0001; // 0000 0000 0000 0001
			}
			// ZF = 0 CF = 0
			else{
				real_machine -> cpu.sf &= 0xfffd;
				real_machine -> cpu.sf &= 0xfffe;
			}

			real_machine -> vm[virtual_machine_index].pc += MEM_WORD_SIZE;
			break;
		}
		// CRRB
		// rb == ra
		case 0x4352:{
			uint16_t reg = command & 0x0000ffff;
			uint8_t reg_nr = get_register_num(reg);

			if(reg_nr != RB){
					real_machine -> cpu.pi = RM_PI_INVALID_ASSIGNMENT;
					break;
			}


			// ZF = 1 CF = 0
			if(real_machine -> cpu.rb == real_machine -> cpu.ra){
				real_machine -> cpu.sf ^= 0x0002; // 0000 0000 0000 0010
				real_machine -> cpu.sf &= 0xfffe; // 1111 1111 1111 1110
			}
			// ZF = 0 CF = 1
			else if(real_machine -> cpu.rb < real_machine -> cpu.ra){
				real_machine -> cpu.sf &= 0xfffd; // 1111 1111 1111 1101
				real_machine -> cpu.sf ^= 0x0001; // 0000 0000 0000 0001
			}
			// ZF = 0 CF = 0
			else{
				real_machine -> cpu.sf &= 0xfffd;
				real_machine -> cpu.sf &= 0xfffe;
			}
			
			real_machine -> vm[virtual_machine_index].pc += MEM_WORD_SIZE;
			break;
		}
		// cycle commands 
		// LOxy
		case 0x4c4f: {
			uint8_t x = char_hex_to_decimal((command & 0x0000ff00) >> 8);
			uint8_t y = char_hex_to_decimal(command & 0x000000ff);
			if(real_machine -> cpu.rc > 0) {
				if(x > 0xf || y > 0xf || x * 16 + y >= MEM_MAX_USER_VM_ADDRESS) {
					real_machine -> cpu.pi = RM_PI_INVALID_ADDRESS;
					break;
				}
				
				--(real_machine -> cpu.rc);
				(real_machine -> vm[virtual_machine_index].pc) = x * 16 + y; 
			}
			else {
				real_machine -> vm[virtual_machine_index].pc += MEM_WORD_SIZE;
			}
			break;
		}
		// logic operations
		// XRa
		// a XOR RA
		case 0x5852:{
			uint8_t valid = 1;
			uint16_t reg = command & 0x0000ffff;
			uint8_t reg_nr = get_register_num(reg);

			if(reg == CPU_UNKNOWN_REGISTER){
				real_machine -> cpu.pi = RM_PI_INVALID_ASSIGNMENT;
				break;
			}

			switch (reg_nr){
				case RA:
					real_machine -> cpu.ra ^= real_machine -> cpu.ra; 
					break;
				case RB:
					real_machine -> cpu.ra ^= real_machine -> cpu.rb;
					break;
				case RC:
					real_machine -> cpu.ra ^= real_machine -> cpu.rc;
					break;
				default:
					real_machine -> cpu.pi = RM_PI_INVALID_ASSIGNMENT;
					valid = 0;
					break;
			}

			if(valid){
				real_machine -> vm[virtual_machine_index].pc += MEM_WORD_SIZE;
			}
			break;
		}
		// ANa
		// a AND RA
		case 0x414e:{
			uint8_t valid = 1;
			uint16_t reg = command & 0x0000ffff;
			uint8_t reg_nr = get_register_num(reg);
			
			if(reg == CPU_UNKNOWN_REGISTER){
				real_machine -> cpu.pi = RM_PI_INVALID_ASSIGNMENT;
				break;
			}

			switch (reg_nr)
			{
				case RA:
					real_machine -> cpu.ra &= real_machine -> cpu.ra;
					break;
				case RB:
					real_machine -> cpu.ra &= real_machine -> cpu.rb;
					break;
				case RC:
					real_machine -> cpu.ra &= real_machine -> cpu.rb;
					break;
				default:
					real_machine -> cpu.pi = RM_PI_INVALID_ASSIGNMENT;
					valid = 0;
					break;
			
			}

			if(valid){
				real_machine -> vm[virtual_machine_index].pc += MEM_WORD_SIZE;
			}
			break;
		}
		// NOa
		// NOT a
		case 0x4e4f:{
			uint8_t valid = 1;
			uint16_t reg = command & 0x0000ffff;
			uint8_t reg_nr = get_register_num(reg);

			if(reg == CPU_UNKNOWN_REGISTER){
				real_machine -> cpu.pi = RM_PI_INVALID_ASSIGNMENT;
				break;
			}

			switch (reg_nr)
			{
				case RA:
					real_machine -> cpu.ra = ~real_machine -> cpu.ra;
					break;
				case RB:
					real_machine -> cpu.rb = ~real_machine -> cpu.rb;
					break;
				case RC:
					real_machine -> cpu.rc = ~real_machine -> cpu.rc;
					break;
				default:
					real_machine -> cpu.pi = RM_PI_INVALID_ASSIGNMENT;
					valid = 0;
					break;					
			}

			if(valid){
				real_machine -> vm[virtual_machine_index].pc += MEM_WORD_SIZE;
			}
			break;

		}

		// JUMPS
		// JUxy
		case 0x4a55: {
			uint8_t x = char_hex_to_decimal((command & 0x0000ff00) >> 8);
			uint8_t y = char_hex_to_decimal(command & 0x000000ff);
			if(x > 0xf || y > 0xf || x * 16 + y >= MEM_MAX_USER_VM_ADDRESS) {
				real_machine -> cpu.pi = RM_PI_INVALID_ADDRESS;
				break;
			}
			
			real_machine -> vm[virtual_machine_index].pc = x * 16 + y;
			break;
		}
		// JZxy
		// control to address x*16+y  if ZF=1
		case 0x4a5a:{
			uint8_t x = char_hex_to_decimal((command & 0x0000ff00) >> 8);
			uint8_t y = char_hex_to_decimal(command & 0x000000ff);
			if(x > 0xf || y > 0xf || x * 16 + y >= MEM_MAX_USER_VM_ADDRESS) {
				real_machine -> cpu.pi = RM_PI_INVALID_ADDRESS;
				break;
			}

			if(((real_machine -> cpu.sf & 0x0002) >> 1) == 1){
				real_machine -> vm[virtual_machine_index].pc = x * 16 + y;
			}


			break;
		}
		// JAxy
		// control to address x*16 + y if CF = 0 ZF = 0
		case 0x4a41: {
			uint8_t x = char_hex_to_decimal((command & 0x0000ff00) >> 8);
			uint8_t y = char_hex_to_decimal(command & 0x000000ff);
			if(x > 0xf || y > 0xf || x * 16 + y >= MEM_MAX_USER_VM_ADDRESS) {
				real_machine -> cpu.pi = RM_PI_INVALID_ADDRESS;
				break;
			}

			if(((real_machine -> cpu.sf & 0x0003)) == 0){
				real_machine -> vm[virtual_machine_index].pc = x * 16 + y;
			}

			break;

		}
		// JBxy
		// control to address x*16 + y if CF = 0
		case 0x4a42: {
			uint8_t x = char_hex_to_decimal((command & 0x0000ff00) >> 8);
			uint8_t y = char_hex_to_decimal(command & 0x000000ff);
			if(x > 0xf || y > 0xf || x * 16 + y >= MEM_MAX_USER_VM_ADDRESS) {
				real_machine -> cpu.pi = RM_PI_INVALID_ADDRESS;
				break;
			}

			if(((real_machine -> cpu.sf & 0x0001)) == 1){
				real_machine -> vm[virtual_machine_index].pc = x * 16 + y;
			}

			break;
		}

		// JNxy
		// control to address x*16 + y if ZF = 0
		case 0x4a4e: {
			uint8_t x = char_hex_to_decimal((command & 0x0000ff00) >> 8);
			uint8_t y = char_hex_to_decimal(command & 0x000000ff);
			if(x > 0xf || y > 0xf || x * 16 + y >= MEM_MAX_USER_VM_ADDRESS) {
				real_machine -> cpu.pi = RM_PI_INVALID_ADDRESS;
				break;
			}
		
			if(((real_machine -> cpu.sf & 0x0002)) == 0){
				real_machine -> vm[virtual_machine_index].pc = x * 16 + y;
			}

			break;
		}

		//STOP
		case 0x5354:{
			uint16_t x = command & 0x0000ffff;

			if(x != 0x4f50){
				real_machine -> cpu.pi = RM_PI_INVALID_ASSIGNMENT;
				break;			}
			else{
				real_machine -> cpu.si = RM_SI_STOP;
			}

			break;
		}
		default:
			real_machine -> cpu.pi = RM_PI_INVALID_OPCODE;
			break;
		}

	
	return 0;
}

int add_virtual_machine(Real_machine* real_machine, Virtual_machine* virtual_machine) {
	if(!real_machine || !virtual_machine) {
		return -1;
	} 

	if(real_machine -> vm_count >= RM_VM_MAX_COUNT) {
		return -1;
	}

	real_machine -> vm[real_machine -> vm_count] = *virtual_machine;
	++(real_machine -> vm_count);
	return 0;
}

int copy_virtual_machine(Real_machine* real_machine, uint8_t virtual_machine_index) {
	if(!real_machine) {
		return -1;
	}

	if(!real_machine -> vm) {
		return -1;
	}

	if(virtual_machine_index >= real_machine -> vm_count) {
		return -1;
	}

	// cannot copy dead machines status
	if((real_machine -> vm[virtual_machine_index]).status == 0) {
		return -1;
	}

	real_machine -> cpu.ra = real_machine -> vm[virtual_machine_index].ra;
	real_machine -> cpu.rb = real_machine -> vm[virtual_machine_index].rb;
	real_machine -> cpu.rc = real_machine -> vm[virtual_machine_index].rc;
	real_machine -> cpu.sf = real_machine -> vm[virtual_machine_index].sf;

	// translate pc to its real value using addresing table
	uint16_t v_addr = real_machine -> vm[virtual_machine_index].pc;
	uint8_t pg_index = real_machine -> vm[virtual_machine_index].page_table_index;
		
	uint16_t real_address = translate_to_real_address(real_machine, v_addr, pg_index);	

	if(real_address == VM_REAL_MACHINE_NULL || real_address == VM_MAX_VIRTUAL_ADDDRESS_EXCEEDED) {
		return -1;
	}
	
	// swap out the page to the real page
	real_machine -> cpu.pc = real_address;

	return 0;
}

int write_virtual_machine(Real_machine* real_machine, uint8_t virtual_machine_index) {
	if(!real_machine) {
		return -1;
	}

	if(!real_machine -> vm) {
		return -1;
	}

	if(virtual_machine_index >= real_machine -> vm_count) {
		return -1;
	}

	// cannot write to a killed machine
	if((real_machine -> vm[virtual_machine_index]).status == 0) {
		return -1;
	}

	real_machine -> vm[virtual_machine_index].ra = real_machine -> cpu.ra;
	real_machine -> vm[virtual_machine_index].rb = real_machine -> cpu.rb;
	real_machine -> vm[virtual_machine_index].rc = real_machine -> cpu.rc;
	real_machine -> vm[virtual_machine_index].sf = real_machine -> cpu.sf;

	// increment virtual machines pc by word size since command was completed
	// real_machine -> vm[virtual_machine_index].pc += MEM_WORD_SIZE;
	// translate the address back to virtual memory
	
	return 0;
}



// not implemented yet
int destroy_real_machine(Real_machine* real_machine) {
	if(!real_machine) {
		return -1;
	}	
	
	close_hard_disk(&(real_machine -> hd));
	free(real_machine -> vm);
	real_machine -> vm = NULL;

	return 0;
}


uint16_t translate_to_real_address(Real_machine *real_machine, uint16_t virtual_address, uint8_t page_table_index) {
	if(!real_machine) {
		return VM_REAL_MACHINE_NULL;
	}

	if(virtual_address >= VM_MAX_VIRTUAL_ADDRESS) {
		return VM_MAX_VIRTUAL_ADDDRESS_EXCEEDED;
	}

	if(page_table_index >= MEM_USER_PAGE_COUNT) {
		return RM_USER_VM_PAGE_INDEX_EXCEEDED;
	}

	uint16_t v_page = (virtual_address / MEM_WORD_SIZE) / 16;
	
	// offset from the virtual page
	uint16_t offset = virtual_address - (v_page * MEM_WORD_SIZE * 16);
	
	// find the corresponfing real page index
	uint16_t r_page = real_machine -> mem.memory[page_table_index * 16 + v_page] & 0x0000ffff;
	
 	return (r_page * 16 * 4) + offset;
}

int remove_virtual_machine(Real_machine* real_machine, uint8_t virtual_machine_index) {
	if(!real_machine) {
		return -1;
	}

	if(virtual_machine_index >= real_machine -> vm_count) {
		return -1;
	}

	if(destroy_virtual_machine(real_machine, &(real_machine -> vm[virtual_machine_index])) != 0) {
		return -1;
	}
	
	// move reposition other machines in its place
	for(uint8_t i = virtual_machine_index; i < real_machine -> vm_count - 1; ++i) {
		real_machine -> vm[i] = real_machine -> vm[i + 1];
	}

	--(real_machine -> vm_count);

	return 0;
}

// page_table_index is used for pstr to translate address
int xchg(Real_machine* real_machine, uint8_t page_table_index) {
	// "Performancas nesvarbu svarbu, svarbu, kad veiktu" - M. Grubliauskis 22/09/2025
	if(!real_machine) {
		return -1;
	}
	
	// used by LWxy command of register must contain the address 
	if(real_machine -> ch_dev.dt == RA_REG && real_machine -> ch_dev.st == USER_MEM) {
		real_machine -> cpu.ra = read_word(&(real_machine -> mem), real_machine -> ch_dev.of);	
		return 0;
	}
		
	// used by SWxy command of register must contain the address
	if(real_machine -> ch_dev.dt == USER_MEM && real_machine -> ch_dev.st == RA_REG) {
		return write_word(&(real_machine -> mem), real_machine -> ch_dev.of, real_machine -> cpu.ra); 
	}

	// BPxy xy must be between 0 - 1F and contained in OF
	if(real_machine -> ch_dev.dt == RA_REG && real_machine -> ch_dev.st == SHARED_MEM) {
		if(real_machine -> cpu.mr != 1) {
			return -1;
		}		
		
		
		if(real_machine -> ch_dev.of >=  MEM_MAX_SHARED_ADDRESS) {
			return -1;
		}
		
		real_machine -> cpu.ra = read_word(&(real_machine -> mem), real_machine -> ch_dev.of + MEM_BEG_SHARED_MEM);	

		real_machine -> cpu.ss = 0;
		return 0;
	}
	
	// BGxy xy must be between 0 - 1F and contained in OF 
	if(real_machine -> ch_dev.dt == SHARED_MEM && real_machine -> ch_dev.st == RA_REG) {
		if(real_machine -> cpu.mr != 1) {
			return -1;
		}
	
		if(real_machine -> ch_dev.of >=  MEM_MAX_SHARED_ADDRESS) {
			return -1;
		}

		if(write_word(&(real_machine -> mem), real_machine -> ch_dev.of + MEM_BEG_SHARED_MEM, real_machine -> cpu.ra) == 0){
			real_machine -> cpu.ss = 0;
			return 0;
		}
		else{
			real_machine -> cpu.ss = 0;
			return -1;
		}

	}
	
	// GEDA 
	if(real_machine -> ch_dev.dt == RA_REG && real_machine -> ch_dev.st == IO_STREAM) {
		if(scanf("%x", &(real_machine -> cpu.ra)) != 1) {
			return -1;
		}
	
		return 0;
	}
	
	// PUTA 
	if(real_machine -> ch_dev.dt == IO_STREAM && real_machine -> ch_dev.st == RA_REG) {
		printf("%x", real_machine -> cpu.ra);	
	
		return 0;
	}
	
	// HDxy OF must contain the xy value
	if(real_machine -> ch_dev.dt == USER_MEM && real_machine -> ch_dev.st == DISK_MEM) {
		uint32_t count = real_machine -> ch_dev.cb / MEM_WORD_SIZE;
		uint8_t rem = real_machine -> ch_dev.cb % MEM_WORD_SIZE;
		uint32_t addr_hd = real_machine -> cpu.ra;
		uint16_t v_addr = real_machine -> ch_dev.of; 		
		uint16_t r_addr;	
	
		for(uint32_t i = 0; i < count; ++i) {
			r_addr = translate_to_real_address(real_machine, v_addr, page_table_index);
			uint32_t word = read_word_hard_disk(&(real_machine -> hd), addr_hd);
			if(write_word(&(real_machine -> mem), r_addr, word) != 0) {
				return -1;
			}
			addr_hd += MEM_WORD_SIZE;
			v_addr += MEM_WORD_SIZE;
		}	
	
		if(rem != 0) {
			uint32_t word = 0;
			
			for(uint8_t i = 0; i < rem; ++i) {
				word |= (read_byte_hard_disk(&(real_machine -> hd), addr_hd) << (8 * i));
				++addr_hd;
			}
		
			if(write_word(&(real_machine -> mem), r_addr, word) != 0) {
				return -1;
			}
		}
	
		return 0;
	}
	
	// HDxy OF must contain the xy value
	if(real_machine -> ch_dev.dt == USER_MEM && real_machine -> ch_dev.st == DISK_MEM) {
		uint32_t count = real_machine -> ch_dev.cb / MEM_WORD_SIZE;
		uint8_t rem = real_machine -> ch_dev.cb % MEM_WORD_SIZE;
		uint32_t addr_hd = real_machine -> cpu.ra;
		uint16_t v_addr = real_machine -> ch_dev.of;	
		uint16_t r_addr;
	
		for(uint32_t i = 0; i < count; ++i) {
			uint16_t r_addr = translate_to_real_address(real_machine, v_addr, page_table_index);
			uint32_t word = read_word(&(real_machine -> mem), r_addr);
			if(write_word_hard_disk(&(real_machine -> hd), addr_hd, word) != 0) {
				return -1;
			}
			addr_hd += MEM_WORD_SIZE;
			v_addr += MEM_WORD_SIZE;
		}
	
		if(rem != 0) {
			uint32_t word = read_word(&(real_machine -> mem), r_addr);
			for(uint8_t i = 0; i < rem; ++i) {
				if(write_byte_hard_disk(&(real_machine -> hd), addr_hd, (word >> (MEM_WORD_SIZE - i)) & 0xff) != 0) {
					return -1;
				}
			}
		}
		
		return 0;
	}
	
	// PSTR prints a char array from the addrress saved in RA, RC, (CB) says how many bytes x
	// doesnt work if the pages are not linear need to translate inside this function
	if(real_machine -> ch_dev.dt == IO_STREAM && real_machine -> ch_dev.st == USER_MEM) {
		// check if it all fits in user memory
		if(real_machine -> cpu.ra + real_machine -> ch_dev.cb >= MEM_MAX_USER_ADDRESS) {
			return -1;
		} 
		
		uint32_t size = (real_machine -> ch_dev.cb) / MEM_WORD_SIZE;
		uint8_t rem = (real_machine -> ch_dev.cb) % MEM_WORD_SIZE;
		uint32_t addr = real_machine -> cpu.ra;
		uint16_t r_addr = translate_to_real_address(real_machine, addr, page_table_index);	
	
		if(rem != 0) {
			++size;
		}

		uint32_t byte_count = real_machine -> ch_dev.cb;
		for(uint32_t i = 0; i < size; ++i) {
			uint16_t r_addr = translate_to_real_address(real_machine, addr, page_table_index);	
			uint32_t word = read_word(&(real_machine -> mem), r_addr);
			addr += MEM_WORD_SIZE;
			for(uint8_t b = 0; b < MEM_WORD_SIZE && byte_count > 0; ++b) {
				uint8_t ch = (word >> (8 * (MEM_WORD_SIZE - 1 - b))) & 0xff;
				putchar(ch);
				--byte_count;
			}
		}
		
		return 0;
	}

	return 0;
}

void pi_si_reset(Real_machine* real_machine) {
	if(!real_machine) {
		return;
	}

	real_machine -> cpu.si = 0;
	real_machine -> cpu.pi = 0;
}
