/************************************************************************/
/* File Name : lc4_disassembler.c 										*/
/* Purpose   : This file implements the reverse assembler 				*/
/*             for LC4 assembly.  It will be called by main()			*/
/*             															*/
/* Author(s) : tjf and you												*/
/************************************************************************/

#include <stdio.h>
#include "lc4_hash.h"

//rd 
#define INSN_11_9(I) (((I) >> 9) & 0x7)

//rs 
#define INSN_8_6(I) (((I) >> 6) & 0x7)

//subcode 
#define INSN_5_3(I) (((I) >> 3) & 0x7)

//subcode 
#define INSN_ADD_IMM(I) (((I) >> 5) & 0x1)

//subcode - cmp  
#define INSN_8_7(I) (((I) >> 7) & 0x3) 

#define INSN_6_0(I) ((I) & 0x7F) 

#define INSN_5_4(I) (((I) >> 4) & 0x3)

#define INSN_OPC(I) (((I) >> 12) & 0x000F)

//rt 
#define INSN_2_0(I) (((I) >> 0) & 0x7)

//sign extend function 
signed short int ext(unsigned short int num, int length) {
    unsigned short int n = num & ((1 << length) - 1);
    signed short int s = (n >> (length - 1)) & 0x0001;
    if (s == 0) {
        return n;
    } else {
        int mask = (1 << (length - 1)); 
				signed short int ans = (n & ~mask) - (mask); 
        return ans;
    }
}

//broke down each assembly function into helper methods 
int assembly_math (unsigned short int content, char* cont, row_of_memory* currNode) {
	//allocate assembly string 
	cont = malloc((20) * sizeof(char)); 
	if (cont == NULL) {
			return 1; 
		}

	//extract all values from binary instruction 
	unsigned short int rd = INSN_11_9(content); 
    unsigned short int rs = INSN_8_6(content); 
    unsigned short int sub_code = INSN_5_3(content); 
	signed short int add_imm = INSN_ADD_IMM(content); 
	unsigned short int rt = INSN_2_0(content);  
	signed short int imm; 

	//divide based on sub op code 
	if (sub_code == 0) {
		sprintf(cont, "ADD R%d R%d R%d", rd, rs, rt); 
	} else if (sub_code == 1) {
		sprintf(cont, "MUL R%d R%d R%d", rd, rs, rt); 
	} else if (sub_code == 2) {
		sprintf(cont, "SUB R%d R%d R%d", rd, rs, rt); 
	} else if (sub_code == 3) {
		sprintf(cont, "DIV R%d R%d R%d", rd, rs, rt); 
	} else if (add_imm == 1) {
		imm = ext(content, 5); 
		sprintf(cont, "ADD R%d R%d #%d", rd, rs, imm); 
	}
	else {
		free(cont); 
		return 1; 
	}

	//update assembly in the node 
	currNode->assembly = cont; 
	return 0; 
}

int assembly_logic (unsigned short int content, char* cont, row_of_memory* currNode) {
	//allocate string for assembly 
	cont = malloc((20) * sizeof(char)); 
	if (cont == NULL) {
			return 1; 
	}

	//extract all values from binary 
	unsigned short int rd = INSN_11_9(content); 
    unsigned short int rs = INSN_8_6(content); 
    unsigned short int sub_code = INSN_5_3(content); 
	signed short int add_imm = INSN_ADD_IMM(content); 
	unsigned short int rt = INSN_2_0(content);  
	signed short int imm; 

	//divide based on sub op code 
	if (sub_code == 0) {
		sprintf(cont, "AND R%d R%d R%d", rd, rs, rt); 
	} else if (sub_code == 1) {
		sprintf(cont, "NOT R%d R%d", rd, rs); 
	} else if (sub_code == 2) {
		sprintf(cont, "OR R%d R%d R%d", rd, rs, rt); 
	} else if (sub_code == 3) {
		sprintf(cont, "XOR R%d R%d R%d", rd, rs, rt); 
	} else if (add_imm == 1) {
		imm = ext(content, 5); 
		sprintf(cont, "AND R%d R%d #%d", rd, rs, imm); 
	}
	else {
		free(cont); 
		return 1; 
	}

	//update assembly in node 
	currNode->assembly = cont; 
	return 0; 
}

int assembly_branch (unsigned short int content, char* cont, row_of_memory* currNode, lc4_memory_segmented* memory) {
	unsigned short int sub_code = INSN_11_9(content); 
	signed short int imm = ext(content, 9);

	cont = malloc((20) * sizeof(char)); 
	if (cont == NULL) {
			return 1; 
		}
	
	//divide based on sub codes 
	if (sub_code == 0) {
		sprintf(cont, "NOP"); 
	} else if (sub_code == 4){
		//finds the label based on the address (PC + 1 + imm)
		unsigned short int add = currNode->address + 1 + imm; 
		char* label = ""; 
		//prints out the label of the node at the found address 
		row_of_memory* found = search_tbl_by_address(memory, add); 
		if (found != NULL && found->label != NULL) {
			label = found->label; 
		}
		sprintf(cont, "BRn %s", label); 
	} else if (sub_code == 6) {
		unsigned short int add = currNode->address + 1 + imm; 
		char* label = ""; 
		row_of_memory* found = search_tbl_by_address(memory, add); 
		if (found != NULL && found->label != NULL) {
			label = found->label; 
		}
		sprintf(cont, "BRnz %s", label); 
	} else if (sub_code == 5) {
		unsigned short int add = currNode->address + 1 + imm; 
		char* label = ""; 
		row_of_memory* found = search_tbl_by_address(memory, add); 
		if (found != NULL && found->label != NULL) {
			label = found->label; 
		}
		sprintf(cont, "BRnp %s", label); 
	} else if (sub_code == 2) {
		unsigned short int add = currNode->address + 1 + imm; 
		char* label = ""; 
		row_of_memory* found = search_tbl_by_address(memory, add); 
		if (found != NULL && found->label != NULL) {
			label = found->label; 
		}
		sprintf(cont, "BRz %s", label); 
	} else if (sub_code == 3) {
		unsigned short int add = currNode->address + 1 + imm; 
		char* label = ""; 
		row_of_memory* found = search_tbl_by_address(memory, add); 
		if (found != NULL && found->label != NULL) {
			label = found->label; 
		}
		sprintf(cont, "BRzp %s", label); 
	} else if (sub_code == 1) {
		unsigned short int add = currNode->address + 1 + imm; 
		char* label = ""; 
		row_of_memory* found = search_tbl_by_address(memory, add); 
		if (found != NULL && found->label != NULL) {
			label = found->label; 
		}
		sprintf(cont, "BRp %s", label); 
	} else if (sub_code == 7) {
		unsigned short int add = currNode->address + 1 + imm; 
		char* label = ""; 
		row_of_memory* found = search_tbl_by_address(memory, add); 
		if (found != NULL && found->label != NULL) {
			label = found->label; 
		}
		sprintf(cont, "BRnzp %s", label); 
	} else {
		return 1; 
	}

	//update assembly in node 
	currNode->assembly = cont; 
	return 0;
}

int assembly_compare (unsigned short int content, char* cont, row_of_memory* currNode) {
	 //read through registers 
    unsigned short int rs = INSN_11_9(content); 
    unsigned short int sub_code = INSN_8_7(content); 
	signed short int imm = ext(content, 7);
	unsigned short int imm_u = INSN_6_0(content); 
	unsigned short int rt = INSN_2_0(content);  

	cont = malloc((20) * sizeof(char)); 
	if (cont == NULL) {
			return 1; 
	}

    //handle function based on sub op code 
	if (sub_code == 0) {
		sprintf(cont, "CMP R%d #R%d", rs, rt); 
    } else if (sub_code == 1) {
		sprintf(cont, "CMPU R%d #R%d", rs, rt); 
    } else if (sub_code == 2) {
		sprintf(cont, "CMPI R%d #%d", rs, imm); 
    } else if (sub_code == 3) {
		sprintf(cont, "CMPIU R%d #%d", rs, imm_u); 
	} else {
		return 1; 
	}

	//update assembly in node 
	currNode->assembly = cont; 
	return 0;

}

int assembly_jsr (unsigned short int content, char* cont, row_of_memory* currNode, lc4_memory_segmented* memory) {
 		unsigned short int type = (content & 0x0800) >> 11; 
		unsigned short int rs = INSN_8_6(content); 

		signed short int imm = ext(content, 11);

		cont = malloc((20) * sizeof(char)); 
		if (cont == NULL) {
			return 1; 
		}

		//divide on subcode 
		if (type == 0) {
			sprintf(cont, "JSRR R%d", rs); 
		} else if (type == 1) {
			//finds label based on address - PC & 0x8000 | imm << 4 
			unsigned short int add = (currNode->address & 0x8000) | (imm << 4); 
			char* label = ""; 
			//prints the label of the node found at that address 
			row_of_memory* found = search_tbl_by_address(memory, add); 
			if (found != NULL && found->label != NULL) {
				label = found->label; 
			}
			sprintf(cont, "JSR %s", label); 
		} else {
			return 1; 
		}

	//update assembly in node 
	currNode->assembly = cont; 
	return 0;	

}

int assembly_ldr (unsigned short int content, char* cont, row_of_memory* currNode) {
	unsigned short int rd = INSN_11_9(content); 
    unsigned short int rs = INSN_8_6(content); 
	signed short int imm = ext(content, 6); 
	cont = malloc((20) * sizeof(char)); 
	if (cont == NULL) {
			return 1; 
	}
	sprintf(cont, "LDR R%d R%d #%d", rd, rs, imm); 

	//update assembly in node 
	currNode->assembly = cont; 
	return 0;	
}

int assembly_str (unsigned short int content, char* cont, row_of_memory* currNode) {
	unsigned short int rt = INSN_11_9(content); 
    unsigned short int rs = INSN_8_6(content); 
	signed short int imm = ext(content, 6); 
	cont = malloc((20) * sizeof(char)); 
	if (cont == NULL) {
			return 1; 
		}
	sprintf(cont, "STR R%d R%d #%d", rt, rs, imm); 

	//update assembly in node 
	currNode->assembly = cont; 
	return 0;	
}

int assembly_RTI (unsigned short int content, char* cont, row_of_memory* currNode) {
	cont = malloc((20) * sizeof(char)); 
	if (cont == NULL) {
			return 1; 
		}
	sprintf(cont, "RTI"); 

	//update assembly in node 
	currNode->assembly = cont; 
	return 0;	
}

int assembly_const (unsigned short int content, char* cont, row_of_memory* currNode) {
	unsigned short int rd = INSN_11_9(content);
	signed short int imm = ext(content, 9);

	cont = malloc((20) * sizeof(char)); 
	if (cont == NULL) {
			return 1; 
		}
	sprintf(cont, "CONST R%d #%d", rd, imm); 

	//update assembly in node 
	currNode->assembly = cont; 
	return 0;	
}

int assembly_hiconst (unsigned short int content, char* cont, row_of_memory* currNode) {
	unsigned short int rd = INSN_11_9(content);
	unsigned short int imm_u =  content & 0x00FF; 

	cont = malloc((20) * sizeof(char)); 
	if (cont == NULL) {
			return 1; 
		}
	sprintf(cont, "HICONST R%d, #%d", rd, imm_u); 

	//update assembly in node 
	currNode->assembly = cont; 
	return 0;	
}

int assembly_shift (unsigned short int content, char* cont, row_of_memory* currNode) {
	unsigned short int rd = INSN_11_9(content); 
	unsigned short int rs = INSN_8_6(content); 
	unsigned short int sub_code = INSN_5_4(content); 
	unsigned short int rt = INSN_2_0(content);  
	unsigned short int imm_u = content & 0x000F;

	cont = malloc((20) * sizeof(char)); 
	if (cont == NULL) {
			return 1; 
		}

//divide on subcode 
	if (sub_code == 0) {
		sprintf(cont, "SLL R%d R%d #%d", rd, rs, imm_u);
	} else if (sub_code == 1) {
		sprintf(cont, "SRA R%d R%d #%d", rd, rs, imm_u);
	} else if (sub_code == 2) {
		sprintf(cont, "SRL R%d R%d #%d", rd, rs, imm_u);
	} else if (sub_code == 3) {
		sprintf(cont, "MOD R%d R%d #R%d", rd, rs, rt);
	} else {
		return 1; 
	}

	//update assembly in node 
	currNode->assembly = cont; 
	return 0;	
}

int assembly_jmp (unsigned short int content, char* cont, row_of_memory* currNode, lc4_memory_segmented* memory) {
	unsigned short int type = ((content & 0x0800) >> 11); 
	unsigned short int rs = INSN_8_6(content); 

	cont = malloc((20) * sizeof(char)); 
	if (cont == NULL) {
			return 1; 
		}

	//divide on subcode 
	if (type == 0) {
		sprintf(cont, "JMPR R%d", rs); 
	} else if (type == 1){
		unsigned short int imm = ext(content, 11);
		//finds label based on address - PC + 1 + imm 
		unsigned short int add = currNode->address + 1 + imm; 
		char* label = ""; 
		//prints label associated with the node found at that address 
		row_of_memory* found = search_tbl_by_address(memory, add); 
		if (found != NULL && found->label != NULL) {
			label = found->label; 
		}
		sprintf(cont, "JMP %s", label); 
	}  else {
		return 1; 
	}

	//update assembly in node 
	currNode->assembly = cont; 
	return 0;
}

int assembly_trap (unsigned short int content, char* cont, row_of_memory* currNode) {
	unsigned short int imm_u =  content & 0x00FF; 
	cont = malloc((20) * sizeof(char)); 
	if (cont == NULL) {
			return 1; 
		}
	sprintf(cont, "TRAP x%04X", imm_u); 

	//update assembly in node 
	currNode->assembly = cont; 
	return 0;	
}

int reverse_assemble (lc4_memory_segmented* memory) 
{
	unsigned short int mathCode = 1; 
	unsigned short int logicCode = 5; 
	unsigned short int brCode = 0; 
	unsigned short int cmpCode = 2; 
	unsigned short int jsrCode = 4; 
	unsigned short int shiftCode = 10; 
	unsigned short int jumpCode = 12; 
	unsigned short int ldrCode = 6; 
	unsigned short int strCode = 7; 
	unsigned short int rtiCode = 8; 
	unsigned short int constCode = 9; 
	unsigned short int hiconstCode = 13; 
	unsigned short int trapCode = 15; 

	row_of_memory* currNode; 
	char* cont; 
	int res; 

	//for each bucket 
	for (int i = 0; i < memory->num_of_buckets; i++) {
		//search for every opcode  
		currNode = search_opcode(memory->buckets[i], mathCode); 
		//check if any more nodes with that opcode 
		while (currNode != NULL) {
			//assigns assembly label 
			res = assembly_math(currNode->contents, cont, currNode); 
			if (res != 0) {
				return 1; 
			}
			//continues searching 
			currNode = search_opcode(memory->buckets[i], mathCode); 
		}
		//if no more with that opcode - we move onto the next one 
		currNode = search_opcode(memory->buckets[i], logicCode);
		while (currNode != NULL) {
			res = assembly_logic(currNode->contents, cont, currNode); 
			if (res != 0) {
				return 1; 
			}
			currNode = search_opcode(memory->buckets[i], logicCode); 
		}
		currNode = search_opcode(memory->buckets[i], brCode);
		while (currNode != NULL) {
			res = assembly_branch(currNode->contents, cont, currNode, memory); 
			if (res != 0) {
				return 1; 
			}
			currNode = search_opcode(memory->buckets[i], brCode); 
		}
		currNode = search_opcode(memory->buckets[i], cmpCode);
		while (currNode != NULL) {
			res = assembly_compare(currNode->contents, cont, currNode); 
			if (res != 0) {
				return 1; 
			}
			currNode = search_opcode(memory->buckets[i], cmpCode); 
		}
		currNode = search_opcode(memory->buckets[i], jsrCode);
		while (currNode != NULL) {
			res = assembly_jsr(currNode->contents, cont, currNode, memory); 
			if (res != 0) {
				return 1; 
			}
			currNode = search_opcode(memory->buckets[i], jsrCode); 
		}
		currNode = search_opcode(memory->buckets[i], shiftCode);
		while (currNode != NULL) {
			res = assembly_shift(currNode->contents, cont, currNode); 
			if (res != 0) {
				return 1; 
			}
			currNode = search_opcode(memory->buckets[i], shiftCode); 
		}
		currNode = search_opcode(memory->buckets[i], jumpCode);
		while (currNode != NULL) {
			res = assembly_jmp(currNode->contents, cont, currNode, memory); 
			if (res != 0) {
				return 1; 
			}
			currNode = search_opcode(memory->buckets[i], jumpCode); 
		}
		currNode = search_opcode(memory->buckets[i], ldrCode);
		while (currNode != NULL) {
			res = assembly_ldr(currNode->contents, cont, currNode);
			if (res != 0) {
				return 1; 
			}
			currNode = search_opcode(memory->buckets[i], ldrCode); 
		}
		currNode = search_opcode(memory->buckets[i], strCode);
		while (currNode != NULL) {
			res = assembly_str(currNode->contents, cont, currNode); 
			if (res != 0) {
				return 1; 
			}
			currNode = search_opcode(memory->buckets[i], strCode); 
		}
		currNode = search_opcode(memory->buckets[i], rtiCode);
		while (currNode != NULL) {
			res = assembly_RTI(currNode->contents, cont, currNode); 
			if (res != 0) {
				return 1; 
			}
			currNode = search_opcode(memory->buckets[i], rtiCode); 
		}
		currNode = search_opcode(memory->buckets[i], constCode);
		while (currNode != NULL) {
			res = assembly_const(currNode->contents, cont, currNode); 
			if (res != 0) {
				return 1; 
			}
			currNode = search_opcode(memory->buckets[i], constCode); 
		}
		currNode = search_opcode(memory->buckets[i], hiconstCode);
		while (currNode != NULL) {
			res = assembly_hiconst(currNode->contents, cont, currNode); 
			if (res != 0) {
				return 1; 
			}
			currNode = search_opcode(memory->buckets[i], hiconstCode); 
		}
		currNode = search_opcode(memory->buckets[i], trapCode);
		while (currNode != NULL) {
			res = assembly_trap(currNode->contents, cont, currNode); 
			if (res != 0) {
				return 1; 
			}
			currNode = search_opcode(memory->buckets[i], trapCode); 
		}
	}

	return 0 ;
}
