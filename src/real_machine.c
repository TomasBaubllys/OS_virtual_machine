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

	return 0;
};

int execute_command(Real_machine* real_machine, uint32_t command) {
	uint16_t com_code = command >> 16;
	uint16_t args = command & 0x0000ffff;				// gets trown away in some cases	

	switch(com_code) {
		// memory commands
		// MOxy
		case 0x4d4f: {
			uint8_t x = char_hex_to_decimal((command & 0x0000ff00) >> 8);
			uint8_t y = char_hex_to_decimal(command & 0x000000ff);
			if(x == 0xff || y == 0xff) {
				return -1;
			}
			
			real_machine -> cpu.ra = x * 16 + y;
			break;
		}
		// LRa 
		// copy RA value to a value
		case 0x4c52: {	
			uint16_t reg = command & 0x0000ffff;
			uint8_t reg_nr = get_register_num(reg);
			if(reg_nr == CPU_UNKNOWN_REGISTER) {
				return -1;
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
					// other registers not allowed
					return -1;
			}			
				
			break;
		}
		// LLa
		// copy a value to RB value
		case 0x4c4c:{
			uint16_t reg = command & 0x0000ffff;
			uint8_t reg_nr = get_register_num(reg);
			if(reg_nr == CPU_UNKNOWN_REGISTER){
				return -1;
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
					return -1;
			}
		}
		// LBxy
		// value at address x*16 + y copying to RA
		// 
	

		// LR 

		// aritmetic commands
		// APxy	
		case 0x4150: {
			uint8_t x = char_hex_to_decimal((command & 0x0000ff00) >> 8);
			uint8_t y = char_hex_to_decimal(command & 0x000000ff);
			if(x == 0xff || y == 0xff) {
				return -1;
			}
			
			real_machine -> cpu.ra = x * 16 + y;
			break;
		}
		// ADa
		// add a value to RA 
		case 0x4144:{
			uint16_t reg = command & 0x0000ffff;
			uint8_t reg_nr = get_register_num(reg);
			if(reg_nr == CPU_UNKNOWN_REGISTER) {
				return -1;
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
					// other registers not allowed
					return -1;
			}			
				
			break;
		}
		// SBa
		// substract a value to RA 
		case 0x5342:{
			uint16_t reg = command & 0x0000ffff;
			uint8_t reg_nr = get_register_num(reg);
			if(reg_nr == CPU_UNKNOWN_REGISTER) {
				return -1;
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
					return -1;
			}			
				
			break;
		}
		// MUa
		// multiplies a value by RA value 
		// carry flag
		// sf 0000011
		case 0x4D55:{
			uint16_t reg = command & 0x0000ffff;
			uint8_t reg_nr = get_register_num(reg);
			if(reg_nr == CPU_UNKNOWN_REGISTER) {
				return -1;
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
					return -1;
			}			
				
			break;
		}
		// DVa
		// dividexs RA value by a value 
		case 0x4456:{
			uint16_t reg = command & 0x0000ffff;
			uint8_t reg_nr = get_register_num(reg);
			if(reg_nr == CPU_UNKNOWN_REGISTER) {
				return -1;
			}

			switch (reg_nr){
				case RA:
					if(real_machine -> cpu.ra == 0){
						real_machine -> cpu.si = 5;
						return 0;
					}
					real_machine -> cpu.ra /= real_machine -> cpu.ra; 
					
					break;
				case RB:
					if(real_machine -> cpu.rb == 0){
						real_machine -> cpu.si = 5;
						return 0;
					}
					uint32_t temp = real_machine -> cpu.ra;
					real_machine -> cpu.ra /= real_machine -> cpu.rb;
					real_machine -> cpu.rb  = temp % real_machine -> cpu.rb;
					break;
				case RC:
					if(real_machine -> cpu.rc == 0){
						real_machine -> cpu.si = 5;
						return 0;
					}
					uint32_t temp1 = real_machine -> cpu.ra;
					real_machine -> cpu.ra /= real_machine -> cpu.rc;
					real_machine -> cpu.rc  = temp1 % real_machine -> cpu.rc;
					break;
				default:
					// other registers not allowed
					return -1;
			}			
				
			break;
		}
		// CMcy
		// (x*16 + y == ra)
		case 0x434d:{
			uint8_t x = char_hex_to_decimal((command & 0x0000ff00) >> 8);
			uint8_t y = char_hex_to_decimal(command & 0x000000ff);
			if(x == 0xff || y == 0xff) {
				return -1;
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

			break;
		}
		// CRRB
		// rb == ra
		case 0x4352:{
			uint16_t reg = command & 0x0000ffff;
			uint8_t reg_nr = get_register_num(reg);

			if(reg_nr != RB){
				return -1;
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

			break;
		}
		// cycle commands 
		// LOxy
		case 0x4c4f: {
			uint8_t x = char_hex_to_decimal((command & 0x0000ff00) >> 8);
			uint8_t y = char_hex_to_decimal(command & 0x000000ff);
			if(real_machine -> cpu.rc > 0) {
				if(x == 0xff || y == 0xff) {
					return -1;
				}
				
				--(real_machine -> cpu.rc);
				(real_machine -> cpu.pc) = x * 16 + y; 
			}
			break;
		}
		// logic operations
		// XRa
		// a XOR RA
		case 0x5852:{
			uint16_t reg = command & 0x0000ffff;
			uint8_t reg_nr = get_register_num(reg);

			if(reg == CPU_UNKNOWN_REGISTER){
				return -1;
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
					return -1;
					break;
			}

			break;
		}
		// ANa
		// a AND RA
		case 0x414e:{
			uint16_t reg = command & 0x0000ffff;
			uint8_t reg_nr = get_register_num(reg);
			
			if(reg == CPU_UNKNOWN_REGISTER){
				return -1;
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
					return -1;
					break;
			}

			break;
		}
		// NOa
		// NOT a
		case 0x4e4f:{
			uint16_t reg = command & 0x0000ffff;
			uint8_t reg_nr = get_register_num(reg);

			if(reg == CPU_UNKNOWN_REGISTER){
				return -1;
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
					return -1;
					break;
			}

			break;

		}

		// JUMPS
		// JUxy
		case 0x4a55: {
			uint8_t x = char_hex_to_decimal((command & 0x0000ff00) >> 8);
			uint8_t y = char_hex_to_decimal(command & 0x000000ff);
			if(x == 0xff || y == 0xff) {
				return -1;
			}
			
			real_machine -> cpu.pc = x * 16 + y;
			break;
		}
		// JZxy
		// control to address x*16+y  if ZF=1
		case 0x4a5a:{
			uint8_t x = char_hex_to_decimal((command & 0x0000ff00) >> 8);
			uint8_t y = char_hex_to_decimal(command & 0x000000ff);
			if(x == 0xff || y == 0xff) {
				return -1;
			}

			if(((real_machine -> cpu.sf & 0x0002) >> 1) == 1){
				real_machine -> cpu.pc = x * 16 + y;
			}

		}


		default:
			return -1;
	}
	
	return 0;
}

int copy_virtual_machine(Real_machine* real_machine, uint8_t virtual_machine_index) {
	if(!real_machine) {
		return -1;
	}

	if(!real_machine -> vm) {
		return -1;
	}

	if(virtual_machine_index >= RM_VM_MAX_COUNT) {
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

	if(virtual_machine_index >= RM_VM_MAX_COUNT) {
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
	real_machine -> vm[virtual_machine_index].pc += MEM_WORD_SIZE;
	
	return 0;
}



// not implemented yet
int destroy_real_machine(Real_machine* real_machine) {
	if(!real_machine) {
		return -1;
	}	

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

