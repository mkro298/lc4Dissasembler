
#ifndef LC4_MEMORY_H
#define LC4_MEMORY_H


typedef struct row_of_memory_struct {
	unsigned short int address ;
	char * label ;
	unsigned short int contents ;
	char * assembly ;
	struct row_of_memory_struct *next ;
} row_of_memory ;


int add_to_list    	          (row_of_memory** head,
			                   unsigned short int address,
			                   unsigned short int contents) ;

row_of_memory* search_address (row_of_memory* head,
			                   unsigned short int address ) ;

row_of_memory* search_opcode  (row_of_memory* head,
			                   unsigned short int opcode  ) ;

int delete_from_list    	  (row_of_memory** head,
			                   unsigned short int address ) ;

void print_list		          (row_of_memory* head,
							   FILE* output_file ) ;

void delete_list    	      (row_of_memory** head ) ;

#endif
