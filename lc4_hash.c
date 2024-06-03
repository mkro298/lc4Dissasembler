
#include <stdio.h>
#include <stdlib.h>
#include "lc4_hash.h"
#include <stdint.h>

/*
 * creates a new hash table with num_of_buckets requested
 */
lc4_memory_segmented* create_hash_table (int num_of_buckets, 
             					         int (*hash_function)(void* table, void *key) ) 
{
	// allocate a single hash table struct
	lc4_memory_segmented* hashTable = (lc4_memory_segmented*) malloc(sizeof(lc4_memory_segmented)); 
	if (hashTable == NULL) {
		return NULL; 
	}

	// allocate memory for the buckets (head pointers)
	hashTable->num_of_buckets = num_of_buckets; 
	row_of_memory** buckets = (row_of_memory**) malloc(sizeof(row_of_memory) * num_of_buckets); 
	if (buckets == NULL) {
		free(hashTable); 
		return NULL;  
	}

	hashTable->buckets = buckets;

	for (int i = 0; i < num_of_buckets; i++) {
		hashTable->buckets[i] = NULL; 
	} 

	// assign function pointer to call back hashing function
	hashTable->hash_function = hash_function; 

	// return 0 for success, -1 for failure
	return  hashTable;
}


/*
 * adds a new entry to the table
 */
int add_entry_to_tbl (lc4_memory_segmented* table, 
					  unsigned short int address,
			    	  unsigned short int contents) 
{
	// apply hashing function to determine proper bucket #
	int hash = table->hash_function(table, (void*)(unsigned long) address); 
	//checks if address already exists 
	row_of_memory* curr = search_tbl_by_address(table, address); 
	//if already there, overwrite by removing it 
	if (curr != NULL) {
		delete_from_list(&table->buckets[hash], address); 
	}
	// add to bucket's linked list using linked list add_to_list() helper function
	int res = add_to_list(&table->buckets[hash], address, contents); 
	if (res == -1) {
		return 1; 
	}
	return 0 ;
}

/*
 * search for an address in the hash table
 */
row_of_memory* search_tbl_by_address 	(lc4_memory_segmented* table,
			                   			 unsigned short int address ) 
{
	// apply hashing function to determine bucket # item must be located in
	int hash = table->hash_function(table, (void*)(unsigned long) address); 
	// invoked linked_lists helper function, search_by_address() to return return proper node
	return search_address(table->buckets[hash], address);
}

/*
 * prints the linked list in a particular bucket
 */

void print_bucket (lc4_memory_segmented* table, 
				   int bucket_number,
				   FILE* output_file ) 
{
	// call the linked list helper function to print linked list
	print_list(table->buckets[bucket_number], output_file); 
	return ;
}

/*
 * print the entire table (all buckets)
 */
void print_table (lc4_memory_segmented* table, 
				  FILE* output_file ) 
{
	/* print out a header to output_file */
	fprintf(output_file, "%-14s %-14s %-14s %-14s\n", "<label>", "<address>", "<contents>", "<assembly>");
	// call the linked list helper function to print linked list to output file for each bucket
	for (int i = 0; i < table->num_of_buckets; i++) {
		print_bucket(table, i, output_file); 
	}
	return ;
}

//helper for the asm file 
void print_helper (row_of_memory* head, FILE* output, int i) {
	if (head == NULL) {
		return; 
	}
	//prints directives based on which bucket we're in 
	if (i == 2 || i == 3) {
		fprintf(output, ".OS\n");
	}
	if (i == 0 || i == 2) {
		fprintf(output, ".CODE\n");
	}
	if (i == 1 || i == 3) {
		fprintf(output, ".DATA\n");
	}
	//address for start of each bucket 
	fprintf(output, ".ADDR x%04X\n", head->address);
	while (head != NULL) {
		//iterates through list and prints ASM 
		if (head->label != NULL) {
			//prints all LABELS 
			fprintf(output, "%s\n", head->label);
		}
		fprintf(output, "%s\n", head->assembly);
		head = head->next; 
	}
	return; 
}

//prints ASM file 
void print_asm (lc4_memory_segmented* table, FILE* output) {
	//goes through each bucket 
	print_helper (table->buckets[0], output, 0); 
	print_helper (table->buckets[1], output, 1); 
	print_helper (table->buckets[2], output, 2); 
	print_helper (table->buckets[3], output, 3); 
	return; 
}


/*
 * delete the entire table and underlying linked lists
 */

void delete_table (lc4_memory_segmented* table ) 
{
	// call linked list delete_list() on each bucket in hash table
	// then delete the table itself
	for (int i = 0; i < table->num_of_buckets; i++) {
		delete_list(&table->buckets[i]); 
	}
	//free table 
	free(table->buckets); 
	free(table); 
	table = NULL; 
	return ;
}
