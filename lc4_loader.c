
#include <stdio.h>
#include <stdlib.h>
#include "lc4_memory.h"
#include "lc4_hash.h"


//swaping function 
unsigned short int swap(unsigned short int a) {
  return (a>>8) | (a<<8); 
}

//opens file 
FILE* open_file(char* file_name)
{
	FILE* file = fopen(file_name, "r"); 
	if (file == NULL) {
		return NULL; 
	}
	return file;
}

int parse_file (FILE* my_obj_file, lc4_memory_segmented* memory)
{
	unsigned short int header; 
	unsigned short int memoryAddress; 
	unsigned short int n; 
	unsigned short int content; 
	row_of_memory* currNode; 
	char *label; 


	//check for if file is done 
	while (fread(&header, sizeof(header), 1, my_obj_file) == 1) {
		//get address and n 
		fread(&memoryAddress, sizeof(memoryAddress), 1, my_obj_file); 
		memoryAddress = swap(memoryAddress); 
		fread(&n, sizeof(n), 1, my_obj_file); 
		n = swap(n); 
		//check for header - either code/data or symbol 
		if (header == 0xdeca || header == 0xdada) {
			//add all n lines to the linked list 
			for (int i = 0; i < n; i++) {
				fread(&content, sizeof(content), 1, my_obj_file);
				content = swap(content); 
				int res = add_entry_to_tbl(memory, memoryAddress, content); 
				if (res != 0) {
					printf("error with adding\n"); 
					fclose(my_obj_file); 
					return 1; 
				}
				memoryAddress++; 
			}
		} else if (header == 0xb7c3){
			label = malloc((n + 1) * sizeof(char)); 
			if (label == NULL) {
				fclose(my_obj_file); 
				return 1; 
			}
			//extract characters into the string 
			for (int i = 0; i < n; i++) {
				label[i] = fgetc(my_obj_file); 
			}
			label[n] = '\0'; 
			//check if node by the label exists 
			currNode = search_tbl_by_address(memory, memoryAddress); 
			if (currNode != NULL) {
				//if it exists, make the new label this label 
				currNode->label = label; 
			}
		}
		else {
			printf("Error in OBJ file"); 
			fclose(my_obj_file); 
			return 1; 
		}
	} 
	//if file is done reading, close and return 
	fclose(my_obj_file); 
	return 0 ;
}
