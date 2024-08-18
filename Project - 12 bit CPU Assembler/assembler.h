/* 
 * File: assembler.h
 *
 * This header file serves as an interface for the main assembler program. It includes necessary
 * dependencies and declares function prototypes used in different phases of the assembler.
 *
 * author: Gal Levi
 * version: 5.8.23
 */

#include "macro_list.h"
#include "labels_BST.h"
#include "ast.h"
#include "encoder.h"
#define CURR_FILE_NAME argv[i] /* current file name that was entered in command line */


/* assembler main used functions prototype */
int pre_assembler(char [], macro_node **, macro_node **);
int first_run(char *, symbol_table_node **, macro_node **, int *, int *, int **, int *);
int second_run(mem_code_word (*)[MAX_MEMORY_ASSUMPTION], int *, mem_data_word (*)[MAX_MEMORY_ASSUMPTION],
				 int *, symbol_table_node *, char *, int *, int);
void export_code_and_data_in_base64(char *, mem_code_word (*code_im)[MAX_MEMORY_ASSUMPTION], int ic,
									 mem_data_word (*data_im)[MAX_MEMORY_ASSUMPTION], int dc);
