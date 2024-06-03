/************************************************************************/
/* File Name : lc4.c 													*/
/* Purpose   : This file contains the main() for this project			*/
/*             main() will call the loader and disassembler functions	*/
/*             															*/
/* Author(s) : tjf and you												*/
/************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "lc4_memory.h"
#include "lc4_hash.h"
#include "lc4_loader.h"
#include "lc4_disassembler.h"

/* program to mimic pennsim loader and disassemble object files */

//hash function 
int hash_func(void *table, void *add) {
	unsigned int key = (unsigned int) (unsigned long) add; 
	//divide based on address
	if (key >= 0x0000 && key <= 0x1FFF) {
		//user code 
		return 0; 
	} else if (key >= 0x2000 && key <= 0x7FFF) {
		//user data 
		return 1; 
	} else if (key >= 0x8000 && key <= 0x9FFF) {
		//OS code
		return 2; 
	} else if (key >= 0xA000 && key <= 0xFFFF) {
		//OS data
		return 3; 
	}
	//error 
    return 4; 
}


int main (int argc, char** argv) {


	/**
	 * main() holds the hashtable &
	 * only calls functions in other files
	 */

	/* step 1: create a pointer to the hashtable: memory 	*/
	lc4_memory_segmented* memory = NULL ;
	char* filename; 

	/* step 2: call create_hash_table() and create 4 buckets 	*/
	memory = create_hash_table(4, &hash_func); 
	if (memory == NULL) {
		printf("error"); 
		return 1; 
	}

	/* step 3: determine filename, then open it		*/
	/*   TODO: extract filename from argv, pass it to open_file() */
	//checks if there's enough arguments 
	if (argc < 3) {
    printf("Error: Not enough arguments"); 
	delete_table(memory); 
    return 1; 
  	}

	/* step 4: call function: parse_file() in lc4_loader.c 	*/
	/*   TODO: call function & check for errors		*/
	int res; 
	/* step 5: repeat steps 3 and 4 for each .OBJ file in argv[] 	*/
	for (int i = 2; i < argc; i++) {
		//opens file 
		FILE* file = open_file(argv[i]); 
		if (file == NULL) {
			printf("Invalid File\n"); 
			delete_table(memory); 
			return 1; 
		}
		//parses file 
		res = parse_file(file, memory); 
		if (res != 0) {
			printf("error parsing file\n"); 
			delete_table(memory); 
			return 1; 
		}
	}


	/* step 6: call function: reverse_assemble() in lc4_disassembler.c */
	/*   TODO: call function & check for errors		*/
	res = reverse_assemble(memory); 
	if (res != 0) {
		printf("error with reverse assemble\n"); 
		delete_table(memory); 
		return 1; 
	}

	/* step 7: call function: print out the hashtable to output file */
	/*   TODO: call function 				*/
	FILE* output = fopen(argv[1], "w"); 
	if (output == NULL) {
		printf("Problem opening file\n"); 
		delete_table(memory); 
		return 1; 
	}
	//prints output 
	print_table(memory, output); 
	fclose(output); 

	//EC create new ASM file - takes the first file arguement and converts that into a .asm file 
	char* filenameasm = argv[1]; 
	char *dot = strrchr(filenameasm, '.'); 
    if (dot != NULL) {
        strcpy(dot, ".asm"); // Replace the extension with the new one
    }
	//opens asm file 
	FILE* outputasm = fopen(filenameasm, "w"); 
	if (outputasm == NULL) {
		delete_table(memory); 
		return 1; 
	}
	//prints out asm file contents 
	print_asm(memory, outputasm); 
	fclose(outputasm); 

	/* step 8: call function: delete_table() in lc4_hash.c */
	/*   TODO: call function & check for errors		*/
	delete_table(memory); 

	/* only return 0 if everything works properly */
	return 0 ;
}
