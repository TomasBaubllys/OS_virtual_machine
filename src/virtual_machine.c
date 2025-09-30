#include "../include/virtual_machine.h"

int init_virtual_machine(Virtual_machine* virtual_machine, CPU* cpu, Memory* memory) {
	if(!virtual_machine || !cpu || !memory) {
		return -1;
	}

	virtual_machine -> cpu = cpu;
	virtual_machine -> memory = memory;

	uint16_t page_table_index = get_free_page(memory);
	
	if(page_table_index == MEM_NO_FREE_PAGE_ERR) {
		return -1;
	}

	virtual_machine -> cpu -> ptr = page_table_index; 
	
	// ask for 16 pages and write them to the page table
	uint16_t page_index;
	for(uint8_t i = 0; i < VM_VIRTUAL_MACHINE_BLOCK_COUNT; ++i) {
		page_index = get_free_page(memory);
		if(page_index == MEM_NO_FREE_PAGE_ERR) {
			// if ran out of pages return everything and return -1;
			for(uint8_t j = 0; j < i; ++j) {
				uint32_t pizza_slice = memory -> memory[page_table_index * 16 + j];
				uint16_t real_page_addr = pizza_slice & 0x00ff;
				return_page(memory, real_page_addr);
			}	
			return_page(memory, page_table_index);
		
			return -1;
		}

		// else write it to the page table
		memory -> memory[page_table_index * 16 + i] = (uint32_t)i << 16 | (uint32_t)page_index; 		
	}
	
	return 0;		
}

// return all the pages	
int destroy_virtual_machine(Virtual_machine* virtual_machine) {
	if(!virtual_machine) {
		return -1;
	}


	for(uint8_t i = 0; i < VM_VIRTUAL_MACHINE_BLOCK_COUNT; ++i) {
	// printf("%d\n", virtual_machine -> rm -> mem.memory[(virtual_machine -> page_table_index) * 16 + i]);
		if(return_page(virtual_machine -> memory, (virtual_machine -> memory -> memory[(virtual_machine -> cpu -> ptr) * 16 + i]) & 0x00ff ) != 0) {
			return -1;
		} 
	}
	
	if(return_page(virtual_machine -> memory, virtual_machine -> cpu -> ptr * 16) != 0) {
		return -1;
	}

	return 0;
}

void virtual_machine_execute(Virtual_machine* virtual_machine) {
	if(!virtual_machine) {
		fprintf(stderr, VM_EXEC_FAILED_NULL_ERR);
		return;
	}

	// translate the address to a real one
	uint16_t com_addr = translate_to_real_address(virtual_machine -> memory, virtual_machine -> cpu -> pc);
	if(com_addr == MEM_NULL_ADDR) {
		virtual_machine -> cpu -> pi = CPU_PI_INVALID_ADDRESS;
		return;
	}


	// read the command
	uint32_t command = read_word(virtual_machine -> memory, com_addr);

	// execute the command
	uint16_t com_code = command >> 16;


	switch(com_code) {
		// memory commands
		// MOxy
		case 0x4d4f: {
			uint8_t x = char_hex_to_decimal((command & 0x0000ff00) >> 8);
			uint8_t y = char_hex_to_decimal(command & 0x000000ff);

			if(x > 0xf || y > 0xf || x * 16 + y >= MEM_MAX_USER_VM_ADDRESS) {
				virtual_machine -> cpu -> pi = CPU_PI_INVALID_ADDRESS;
				break;
			}

			virtual_machine -> cpu -> ra = x * 16 + y;
			virtual_machine -> cpu -> pc += MEM_WORD_SIZE;
			break;
		}
		// LRa
		// copy RA value to a value
		case 0x4c52: {
			uint16_t reg = command & 0x0000ffff;
			reg = uint16_t_to_lower(reg);

			switch (reg){
				// RA
				case RA_CODE:
					// nothing
					break;
				// RB
				case RB_CODE:
					virtual_machine -> cpu -> rb = virtual_machine -> cpu -> ra;
					break;
				// RC
				case RC_CODE:
					virtual_machine -> cpu -> rc = virtual_machine -> cpu -> ra;
					break;
				default:
					virtual_machine -> cpu -> pi = CPU_PI_INVALID_ASSIGNMENT;
					break;
			}

			virtual_machine -> cpu -> pc += MEM_WORD_SIZE;

			break;
		}
		// LLa
		// copy a value to RB value
		case 0x4c4c: {
			uint16_t reg = command & 0x0000ffff;
			reg = uint16_t_to_lower(reg);

			switch (reg){
				case RA_CODE:
					virtual_machine -> cpu -> rb = virtual_machine -> cpu -> ra;
					break;
				case RB_CODE:
					// nothing
					break;
				case RC_CODE:
					virtual_machine -> cpu -> rb = virtual_machine -> cpu -> rc;
					break;
				default:
					virtual_machine -> cpu -> pi = CPU_PI_INVALID_ASSIGNMENT;
					break;
			}

			virtual_machine -> cpu -> pc += MEM_WORD_SIZE;
			break;
		}
		// LWxy handle page missalignment
		// value at address x*16 + y copying to RA
		case 0x4c57: {
			uint8_t x = char_hex_to_decimal((command & 0x0000ff00) >> 8);
			uint8_t y = char_hex_to_decimal(command & 0x000000ff);

			if(x > 0xf || y > 0xf || x * 16 + y >= MEM_MAX_USER_VM_ADDRESS) {
				virtual_machine -> cpu -> pi = CPU_PI_INVALID_ADDRESS;
				break;
			}
			
			virtual_machine -> cpu -> ra &= 0x0000ffff;
			virtual_machine -> cpu -> ra |= (x * 16 + y) << 16; // save the address in the upper 2 bytes of ra
			virtual_machine -> cpu -> si = CPU_SI_LW;

			virtual_machine -> cpu -> pc += MEM_WORD_SIZE;
			break;
		}
		// SWxy handle page missalignmet
		case 0x5357: {
			uint8_t x = char_hex_to_decimal((command & 0x0000ff00) >> 8);
			uint8_t y = char_hex_to_decimal(command & 0x000000ff);

			if(x > 0xf || y > 0xf || x * 16 + y >= MEM_MAX_USER_VM_ADDRESS) {
				virtual_machine -> cpu -> pi = CPU_PI_INVALID_ADDRESS;
				break;
			}

			virtual_machine -> cpu -> rb &= 0x0000ffff;
			virtual_machine -> cpu -> rb |= (x * 16 + y) << 16; // save the address in the upper 2 bytes of rb
			virtual_machine -> cpu -> si = CPU_SI_SW;
			virtual_machine -> cpu -> pc += MEM_WORD_SIZE;
			break;
		}
		// BPxy
		case 0x4250: {
			if(virtual_machine -> cpu -> ss == SEMAFOR_BUSY){
				break;
			}
			else{
				virtual_machine -> cpu -> ss = SEMAFOR_BUSY;
			}
			uint8_t x = char_hex_to_decimal((command & 0x0000ff00) >> 8);
			uint8_t y = char_hex_to_decimal(command & 0x000000ff);

			if(x > 0xf || y > 0xf || x * 16 + y > MEM_MAX_SHARED_ADDRESS) {
				virtual_machine -> cpu -> pi = CPU_PI_INVALID_ADDRESS;
				break;
			}

			virtual_machine -> cpu -> rb &= 0x0000ffff;
			virtual_machine -> cpu -> rb |= (x * 16 + y) << 16;
			virtual_machine -> cpu -> si = CPU_SI_BP;
			virtual_machine -> cpu -> pc += MEM_WORD_SIZE;
			break;
		}
		// BG
		case 0x4247: {
			if(virtual_machine -> cpu -> ss == 1){
				break;
			}
			else{
				virtual_machine -> cpu -> ss = 1;
			}

			uint8_t x = char_hex_to_decimal((command & 0x0000ff00) >> 8);
			uint8_t y = char_hex_to_decimal(command & 0x000000ff);

			if(x > 0xf || y > 0xf || x * 16 + y > MEM_MAX_SHARED_ADDRESS) {
				virtual_machine -> cpu -> pi = CPU_PI_INVALID_ADDRESS;
				break;
			}

			virtual_machine -> cpu -> ra &= 0x0000ffff;
			virtual_machine -> cpu -> ra |= (x * 16 + y) << 16;

			virtual_machine -> cpu -> si = CPU_SI_BP;
			virtual_machine -> cpu -> pc += MEM_WORD_SIZE;
			break;
		}

		// I/O commands
		// GEDA
		case 0x4745: {
			uint16_t rest_com = command & 0x0000ffff;
			if(rest_com != 0x4441) {
				virtual_machine -> cpu -> pi = CPU_PI_INVALID_OPCODE;
				break;
			}

			virtual_machine -> cpu -> si = CPU_SI_GEDA;
			virtual_machine -> cpu -> pc += MEM_WORD_SIZE;
			break;
		}

		// PUTA
		case 0x5055: {
			uint16_t rest_com = command & 0x0000ffff;
			if(rest_com != 0x5441) {
				virtual_machine -> cpu -> pi = CPU_PI_INVALID_OPCODE;
				break;
			}

			virtual_machine -> cpu -> si = CPU_SI_PUTA;
			virtual_machine -> cpu -> pc += MEM_WORD_SIZE;
			break;
		}

		// PSTR
		case 0x5053: {
			uint16_t rest_com = command & 0x0000ffff;
			if(rest_com != 0x5452) {
				virtual_machine -> cpu -> pi = CPU_PI_INVALID_OPCODE;
				break;
			}

			if(virtual_machine -> cpu -> ra >= MEM_MAX_USER_VM_ADDRESS) {
				virtual_machine -> cpu -> pi = CPU_PI_INVALID_ADDRESS;
				break;
			}

			virtual_machine -> cpu -> si = CPU_SI_PSTR;
			virtual_machine -> cpu -> pc += MEM_WORD_SIZE;
			break;
		}

		// aritmetic commands
		// APxy
		case 0x4150: {
			uint8_t x = char_hex_to_decimal((command & 0x0000ff00) >> 8);
			uint8_t y = char_hex_to_decimal(command & 0x000000ff);

			// if overflow encountered continue? or not but set the pi
			if(virtual_machine -> cpu -> ra > 0xffffffff - x * 16 + y) {
				virtual_machine -> cpu -> pi = CPU_PI_OVERFLOW;
				break;
			}

			virtual_machine -> cpu -> ra += x * 16 + y;
			virtual_machine -> cpu -> pc += MEM_WORD_SIZE;
			break;
		}
		// ADa
		// add a value to RA
		case 0x4144: {
			uint16_t reg = command & 0x0000ffff;
			reg = uint16_t_to_lower(reg);

			uint32_t old_ra = virtual_machine -> cpu -> ra;
			switch (reg){
				case RA_CODE:
					virtual_machine -> cpu -> ra += virtual_machine -> cpu -> ra;
					break;
				case RB_CODE:
					virtual_machine -> cpu -> ra += virtual_machine -> cpu -> rb;
					break;
				case RC_CODE:
					virtual_machine -> cpu -> ra += virtual_machine -> cpu -> rc;
					break;
				default:
					virtual_machine -> cpu -> pi = CPU_PI_INVALID_ASSIGNMENT;
					break;
			}

			if(old_ra > virtual_machine -> cpu -> ra) {
				virtual_machine -> cpu -> pi = CPU_PI_OVERFLOW;
			}

			virtual_machine -> cpu -> pc += MEM_WORD_SIZE;
		break;
		}
		// SBa
		// substract a value to RA
		case 0x5342: {
			uint16_t reg = command & 0x0000ffff;
			reg = uint16_t_to_lower(reg);
			uint32_t old_ra = virtual_machine -> cpu -> ra;

			switch (reg){
				case RA_CODE:
					virtual_machine -> cpu -> ra -= virtual_machine -> cpu -> ra;
					break;
				case RB_CODE:
					virtual_machine -> cpu -> ra -= virtual_machine -> cpu -> rb;
					break;
				case RC_CODE:
					virtual_machine -> cpu -> ra -= virtual_machine -> cpu -> rc;
					break;
				default:
					// other registers not allowed
					virtual_machine -> cpu -> pi = CPU_PI_INVALID_ASSIGNMENT;
					break;
			}

			if(old_ra < virtual_machine -> cpu -> ra) {
				virtual_machine -> cpu -> pi = CPU_PI_OVERFLOW;
			}

			virtual_machine -> cpu -> pc += MEM_WORD_SIZE;
			break;
		}
		// MUa
		// multiplies a value by RA value
		// carry flag
		// sf 0000011
		case 0x4D55: {
		uint16_t reg = command & 0x0000ffff;
		reg = uint16_t_to_lower(reg);
		uint32_t old_ra = virtual_machine -> cpu -> ra;

			switch (reg){
				case RA_CODE:
					virtual_machine -> cpu -> ra *= virtual_machine -> cpu -> ra;
					break;
				case RB_CODE:
					virtual_machine -> cpu -> ra *= virtual_machine -> cpu -> rb;
					break;
				case RC_CODE:
					virtual_machine -> cpu -> ra *= virtual_machine -> cpu -> rc;
					break;
				default:
					// other registers not allowed
					virtual_machine -> cpu -> pi = CPU_PI_INVALID_ASSIGNMENT;;
					break;
			}

			if(old_ra > virtual_machine -> cpu -> ra) {
				virtual_machine -> cpu -> pi = CPU_PI_OVERFLOW;
			}

			virtual_machine -> cpu -> pc += MEM_WORD_SIZE;
			break;
		}

		// DVa
		// dividexs RA value by a value
		case 0x4456: {
			uint16_t reg = command & 0x0000ffff;
			reg = uint16_t_to_lower(reg);

			switch (reg){
				case RA_CODE:
					if(virtual_machine -> cpu -> ra == 0){
						virtual_machine -> cpu -> pi = CPU_PI_DIVISION_BY_ZERO;
						break;
					}
					virtual_machine -> cpu -> ra = 1;

					break;
				case RB_CODE:
					if(virtual_machine -> cpu -> rb == 0){
						virtual_machine -> cpu -> pi = CPU_PI_DIVISION_BY_ZERO;
						break;
					}
					uint32_t temp = virtual_machine -> cpu -> ra;
					virtual_machine -> cpu -> ra /= virtual_machine -> cpu -> rb;
					virtual_machine -> cpu -> rb  = temp % virtual_machine -> cpu -> rb;
					break;
				case RC_CODE:
					if(virtual_machine -> cpu -> rc == 0){
						virtual_machine -> cpu -> pi = CPU_PI_DIVISION_BY_ZERO;
						break;
					}
					uint32_t temp1 = virtual_machine -> cpu -> ra;
					virtual_machine -> cpu -> ra /= virtual_machine -> cpu -> rc;
					virtual_machine -> cpu -> rc  = temp1 % virtual_machine -> cpu -> rc;
					break;
				default:
					// other registers not allowed
					virtual_machine -> cpu -> pi = CPU_PI_INVALID_ASSIGNMENT;
					break;
			}

			virtual_machine -> cpu -> pc += MEM_WORD_SIZE;
			break;
		}

		// CMxy
		// (x*16 + y == ra)
		case 0x434d: {
			uint8_t x = char_hex_to_decimal((command & 0x0000ff00) >> 8);
			uint8_t y = char_hex_to_decimal(command & 0x000000ff);
			if(x > 0xf || y > 0xf || x * 16 + y >= MEM_MAX_USER_VM_ADDRESS) {
				virtual_machine -> cpu -> pi = CPU_PI_INVALID_ADDRESS;
				break;
			}

			// ZF = 1 CF = 0
			if(x * 16 + y == virtual_machine -> cpu -> ra){
				virtual_machine -> cpu -> sf ^= 0x0002; // 0000 0000 0000 0010
				virtual_machine -> cpu -> sf &= 0xfffe; // 1111 1111 1111 1110
			}
			// ZF = 0 CF = 1
			else if(x * 16 + y < virtual_machine -> cpu -> ra){
				virtual_machine -> cpu -> sf &=  0xfffd; // 1111 1111 1111 1101
				virtual_machine -> cpu -> sf ^= 0x0001; // 0000 0000 0000 0001
			}
			// ZF = 0 CF = 0
			else{
				virtual_machine -> cpu -> sf &= 0xfffd;
				virtual_machine -> cpu -> sf &= 0xfffe;
			}

			virtual_machine -> cpu -> pc += MEM_WORD_SIZE;
			break;
		}
		// CRRB
		// rb == ra
		case 0x4352: {
			uint16_t reg = command & 0x0000ffff;
			reg = uint16_t_to_lower(reg);

			if(reg != RB_CODE){
				virtual_machine -> cpu -> pi = CPU_PI_INVALID_ASSIGNMENT;
				break;
			}


			// ZF = 1 CF = 0
			if(virtual_machine -> cpu -> rb == virtual_machine -> cpu -> ra){
				virtual_machine -> cpu -> sf ^= 0x0002; // 0000 0000 0000 0010
				virtual_machine -> cpu -> sf &= 0xfffe; // 1111 1111 1111 1110
			}
			// ZF = 0 CF = 1
			else if(virtual_machine -> cpu -> rb < virtual_machine -> cpu -> ra){
				virtual_machine -> cpu -> sf &= 0xfffd; // 1111 1111 1111 1101
				virtual_machine -> cpu -> sf ^= 0x0001; // 0000 0000 0000 0001
			}
			// ZF = 0 CF = 0
			else{
				virtual_machine -> cpu -> sf &= 0xfffd;
				virtual_machine -> cpu -> sf &= 0xfffe;
			}

			virtual_machine -> cpu -> pc += MEM_WORD_SIZE;
			break;
		}
		// cycle commands
		// LOxy
		case 0x4c4f: {
			uint8_t x = char_hex_to_decimal((command & 0x0000ff00) >> 8);
			uint8_t y = char_hex_to_decimal(command & 0x000000ff);
			if(virtual_machine -> cpu -> rc > 0) {
				if(x > 0xf || y > 0xf || x * 16 + y >= MEM_MAX_USER_VM_ADDRESS) {
					virtual_machine -> cpu -> pi = CPU_PI_INVALID_ADDRESS;
					break;
				}

				--(virtual_machine -> cpu -> rc);
				virtual_machine -> cpu -> pc = x * 16 + y;
			}
			else {
				virtual_machine -> cpu -> pc += MEM_WORD_SIZE;
			}
			break;
		}
		// logic operations
		// XRa
		// a XOR RA
		case 0x5852: {
			uint16_t reg = command & 0x0000ffff;
			reg = uint16_t_to_lower(reg);

			switch (reg){
				case RA_CODE:
					virtual_machine -> cpu -> ra = 0;
					break;
				case RB_CODE:
					virtual_machine -> cpu -> ra ^= virtual_machine -> cpu -> rb;
					break;
				case RC_CODE:
					virtual_machine -> cpu -> ra ^= virtual_machine -> cpu -> rc;
					break;
				default:
					virtual_machine -> cpu -> pi = CPU_PI_INVALID_ASSIGNMENT;
					break;
			}

			virtual_machine -> cpu -> pc += MEM_WORD_SIZE;

			break;
		}
		// ANa
		// a AND RA
		case 0x414e: {
			uint16_t reg = command & 0x0000ffff;
			reg = uint16_t_to_lower(reg);

			switch (reg) {
				case RA_CODE:
					virtual_machine -> cpu -> ra &= virtual_machine -> cpu -> ra;
					break;
				case RB_CODE:
					virtual_machine -> cpu -> ra &= virtual_machine -> cpu -> rb;
					break;
				case RC_CODE:
					virtual_machine -> cpu -> ra &= virtual_machine -> cpu -> rc;
					break;
				default:
					virtual_machine -> cpu -> pi = CPU_PI_INVALID_ASSIGNMENT;
					break;

			}
			virtual_machine -> cpu -> pc += MEM_WORD_SIZE;

			break;
		}
		// NOa
		// NOT a
		case 0x4e4f: {
			uint16_t reg = command & 0x0000ffff;
			reg = uint16_t_to_lower(reg);

			switch (reg) {
				case RA_CODE:
					virtual_machine -> cpu -> ra = ~virtual_machine -> cpu -> ra;
					break;
				case RB_CODE:
					virtual_machine -> cpu -> rb = ~virtual_machine -> cpu -> rb;
					break;
				case RC_CODE:
					virtual_machine -> cpu -> rc = ~virtual_machine -> cpu -> rc;
					break;
				default:
					virtual_machine -> cpu -> pi = CPU_PI_INVALID_ASSIGNMENT;
					break;
			}
			virtual_machine -> cpu -> pc += MEM_WORD_SIZE;

			break;
		}
		// JUMPS
		// JUxy
		case 0x4a55: {
			uint8_t x = char_hex_to_decimal((command & 0x0000ff00) >> 8);
			uint8_t y = char_hex_to_decimal(command & 0x000000ff);
			if(x > 0xf || y > 0xf || x * 16 + y >= MEM_MAX_USER_VM_ADDRESS) {
				virtual_machine -> cpu -> pi = CPU_PI_INVALID_ADDRESS;
				break;
			}

			virtual_machine -> cpu -> pc = x * 16 + y;
			break;
		}
		// JZxy
		// control to address x*16+y  if ZF=1
		case 0x4a5a: {
			uint8_t x = char_hex_to_decimal((command & 0x0000ff00) >> 8);
			uint8_t y = char_hex_to_decimal(command & 0x000000ff);
			if(x > 0xf || y > 0xf || x * 16 + y >= MEM_MAX_USER_VM_ADDRESS) {
				virtual_machine -> cpu -> pi = CPU_PI_INVALID_ADDRESS;
				break;
			}

			if(((virtual_machine -> cpu -> sf & 0x0002) >> 1) == 1){
				virtual_machine -> cpu -> pc = x * 16 + y;
			}

			break;
		}
		// JAxy
		// control to address x*16 + y if CF = 0 ZF = 0
		case 0x4a41: {
			uint8_t x = char_hex_to_decimal((command & 0x0000ff00) >> 8);
			uint8_t y = char_hex_to_decimal(command & 0x000000ff);
			if(x > 0xf || y > 0xf || x * 16 + y >= MEM_MAX_USER_VM_ADDRESS) {
				virtual_machine -> cpu -> pi = CPU_PI_INVALID_ADDRESS;
				break;
			}

			if(((virtual_machine -> cpu -> sf & 0x0003)) == 0){
				virtual_machine -> cpu -> pc = x * 16 + y;
			}

			break;
		}
		// JBxy
		// control to address x*16 + y if CF = 0
		case 0x4a42: {
			uint8_t x = char_hex_to_decimal((command & 0x0000ff00) >> 8);
			uint8_t y = char_hex_to_decimal(command & 0x000000ff);
			if(x > 0xf || y > 0xf || x * 16 + y >= MEM_MAX_USER_VM_ADDRESS) {
				virtual_machine -> cpu -> pi = CPU_PI_INVALID_ADDRESS;
				break;
			}

			if(((virtual_machine -> cpu -> sf & 0x0001)) == 1){
				virtual_machine -> cpu -> pc = x * 16 + y;
			}

			break;
		}

		// JNxy
		// control to address x*16 + y if ZF = 0
		case 0x4a4e: {
			uint8_t x = char_hex_to_decimal((command & 0x0000ff00) >> 8);
			uint8_t y = char_hex_to_decimal(command & 0x000000ff);
			if(x > 0xf || y > 0xf || x * 16 + y >= MEM_MAX_USER_VM_ADDRESS) {
				virtual_machine -> cpu -> pi = CPU_PI_INVALID_ADDRESS;
				break;
			}

			if(((virtual_machine -> cpu -> sf & 0x0002)) == 0){
				virtual_machine -> cpu -> pc = x * 16 + y;
			}

			break;
		}
		//STOP
		case 0x5354:{
			uint16_t x = command & 0x0000ffff;

			if(x != 0x4f50){
				virtual_machine -> cpu -> pi = CPU_PI_INVALID_ASSIGNMENT;
				break;			}
				else{
					virtual_machine -> cpu -> si = CPU_SI_STOP;
				}

				break;
		}
		default:
			virtual_machine -> cpu -> pi = CPU_PI_INVALID_OPCODE;
			break;
	}

	--(virtual_machine -> cpu -> ti);

	return 0;
}

/*int load_program_virtual_machine(Virtual_machine* virtual_machine, uint32_t* program, uint16_t program_len) {
	if(!real_machine) {
		return -1;
	}

	if(!program) {
		return -1;
	}

	if(virtual_machine_index >= RM_VM_MAX_COUNT) {
		return -1;
	}

	if(virtual_address >= VM_MAX_VIRTUAL_ADDRESS) {
		return -1;
	}	

	if(virtual_address + program_len * 4 >= VM_MAX_VIRTUAL_ADDRESS) {
		return -1;
	}

	uint16_t page_table_index = real_machine -> vm[virtual_machine_index].page_table_index;

	// write the program to memory
	for(uint16_t i = 0; i < program_len; ++i) {
		// translate virtual_address to a real one
		uint16_t real_address = translate_to_real_address(real_machine, virtual_address, page_table_index);	

		// write to that real address	
		write_word(&(real_machine -> mem), real_address, program[i]); 
	
		// increment virtual address by MEM_WORD_SIZE
		virtual_address += MEM_WORD_SIZE;
	}
	
	reset_virtual_machine_registers((real_machine -> vm) + virtual_machine_index);

	return 0;
}*/

