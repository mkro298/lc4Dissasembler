
#include <stdio.h>
#include <stdlib.h>
#include "lc4_memory.h"

#define INSN_OP(I) ((I) >> 12)

/*
 * adds a new node to linked list pointed to by head
 */
int add_to_list (row_of_memory** head,
		 		 unsigned short int address,
		 		 unsigned short int contents)
{

	/* allocate memory for a single node */
	row_of_memory *mine = malloc(sizeof(row_of_memory)); 
	if (mine == NULL) { 
		return -1; 
	} 

	/* populate fields in newly allocated node with arguments: address/contents */
	mine->address = address; 
	mine->contents = contents;

    /* make certain to set other fields in structure to NULL */
	mine->label = NULL; 
	mine->assembly = NULL; 
	mine->next = NULL; 


	/* if head==NULL, node created is the new head of the list! */
	if (*head == NULL) {
		*head = mine; 
		return 0; 
	}
	/* otherwise, insert the node into the linked list keeping it in order of ascending addresses */
	row_of_memory* prev = *head; 
	row_of_memory* curr = *head; 

	//traverse down the list 
	while (curr != NULL) {
		//check which spot it should be in for ascending order 
		if (mine->address <= curr->address) {
			//check if it's the head - if it is inserting is a little different  
			if (curr == *head) {
				mine->next = *head; 
				*head = mine; 
				return 0; 
			}
			else {
				//otherwise just place inbetween 
				prev->next = mine; 
				mine->next = curr; 
				return 0; 
			}
		}
		//keep traversing otherwise 
		prev = curr; 
		curr = curr->next; 
	}

	//else add at end 
	if (prev != NULL) {
		prev->next = mine; 
	}

	/* return 0 for success, -1 if malloc fails */
	return 0 ;
}


/*
 * search linked list by address field, returns node if found
 */
row_of_memory* search_address (row_of_memory* head,
			        		   unsigned short int address )
{
	/* traverse linked list, searching each node for "address"  */
	if (head == NULL) {
		return NULL; 
	}

	row_of_memory* curr = head; 
	while (curr != NULL) {
		/* return pointer to node in the list if item is found */
		if (curr->address == address) {
			return curr; 
		}
		curr = curr->next; 
	}

	/* return NULL if list is empty or if "address" isn't found */

	return NULL ;
}

/*
 * search linked list by opcode field, returns node if found
 */
row_of_memory* search_opcode (row_of_memory* head,
				      		  unsigned short int opcode  )
{
	/* traverse linked list until node is found with matching opcode
	   AND "assembly" field of node is empty */
	if (head == NULL) {
		return NULL; 
	}

	row_of_memory* curr = head; 
	while (curr != NULL) {
		/* return pointer to node in the list if item is found */
		if (curr->assembly == NULL && INSN_OP(curr->contents) == opcode) {
			return curr; 
		}
		curr = curr->next; 
	}

	/* return NULL if list is empty or if no matching nodes */

	return NULL ;
}

/*
 * delete the node from the linked list with matching address
 */
int delete_from_list (row_of_memory** head,
			          unsigned short int address ) 
{
	/* if head isn't NULL, traverse linked list until node is found with matching address */
	if (*head == NULL) {
		return -1; 
	}

	row_of_memory* curr = *head; 
	row_of_memory* prev = *head; 
	//traverse down to find it 
	while (curr != NULL) {
		//if we find it then we can stop traversing 
		if (curr->address == address) {
			break; 
		}
		prev = curr; 
		curr = curr->next; 
	}

	if (curr == NULL) {
		//node not found 
		return -1; 
	}

	/* make certain to update the head pointer - if original was deleted */
	if (curr == *head) {
		*head = curr->next; 
		free(curr->label); 
		free(curr->assembly); 
		free(curr); 
		return 0; 
	}

	/* delete the matching node, re-link the list */
	prev->next = curr->next; 
	free(curr->label); 
	free(curr->assembly); 
	free(curr); 
	/* return 0 if successfully deleted the node from list, -1 if node wasn't found */

	return 0 ;

}

void print_list (row_of_memory* head, 
				 FILE* output_file )
{
	int i = 0; 
	/* make sure head isn't NULL */
	if (head == NULL) {
		return ; 
	}
 

	/* traverse linked list, print contents of each node to output_file */
	while (head != NULL) {
		char* label = ""; 
		char* assembly = ""; 
		if (head->label != NULL) {
			label = head->label; 
		}
		if (head->assembly != NULL) {
			assembly = head->assembly; 
		}
		fprintf(output_file, "%-14s %04X           %04X           %-14s\n", label, head->address, head->contents, assembly);
		head = head->next;
	}
	return ;
}

/*
 * delete entire linked list
 */
void delete_list (row_of_memory** head )
{
	/* delete entire list node by node */
	/* set head = NULL upon deletion */
	if (*head == NULL) {
		return; 
	}

	//go down entire list and free and delete each node 
	while (*head != NULL) {
		row_of_memory *temp = *head;  
		*head = (*head)->next;  
		free((temp)->assembly); 
		free((temp)->label); 
		free(temp);
	}
	*head = NULL; 
	return ;
}