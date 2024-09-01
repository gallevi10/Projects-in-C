/*
 *	File: second_run.c
 *
 *  This file contains the implementation of the second run phase of the assembly compiler.
 *  In the second run, the assembler processes each line of the input assembly source code,
 *  evaluates instructions and directives, and generates the actual machine code instructions.
 *  It also performs additional checks for entry and extern labels, ensuring they are correctly used.
 *  
 *  author: Gal Levi
 *  version: 5.8.23
 */


#include "labels_BST.h"
#include "ast.h"
#include "encoder.h"

/* functions protoype */
int encoder(mem_code_word (*)[MAX_MEMORY_ASSUMPTION], int *, mem_data_word (*)[MAX_MEMORY_ASSUMPTION],
 			int *, ast *, symbol_table_node *, char *, int);


/*
 *  Performs the second run phase of the assembly compilation process.
 *  In this phase, the function processes each line of the assembly source code,
 *  generates the actual machine code instructions and data memory words,
 *  and performs checks related to entry and extern labels.
 *
 *  param code_im - A pointer to a 2D array of mem_code_word representing the code image.
 *  param ic_add - A pointer to the current instruction counter, which gets updated during the run.
 *  param data_im - A pointer to a 2D array of mem_data_word representing the data image.
 *  param dc_add - A pointer to the current data counter, which gets updated during the run.
 *  param symbol_table - A pointer to the symbol table containing label information.
 *  param file_name - The name of the source assembly file being processed.
 *  param error_lines - An array of integers representing lines with errors from the first run.
 *  param err_ln_size - The size of the error_lines array in bytes.
 *
 *  returns:
 *  - 1 if the assembly compilation process is successful without errors, indicating a valid assembly 
 *	program.
 *  - 0 if errors are encountered during the second run, indicating an invalid assembly program.
 */
int second_run(mem_code_word (*code_im)[MAX_MEMORY_ASSUMPTION], int *ic_add,
				mem_data_word (*data_im)[MAX_MEMORY_ASSUMPTION], int *dc_add,
				 symbol_table_node *symbol_table, char *file_name, int *error_lines, int err_ln_size){
 			
 			
 	int line_num = 0, is_valid = 1, is_line_valid = 1, i;
	FILE *src;
	char src_name[MAX_BUFFER], curr_line[MAX_BUFFER];
	ast curr_line_ast;
	symbol_table_node *curr_search_res;
	
	sprintf(src_name, "%s.am", file_name);
	
	if (!(src = fopen(src_name, "r"))){
	
		errprintf(src_name, NO_LINE_ERROR, "cannot open file - second run");
		return 0;
		
	}
	
	memset(curr_line, '\0', MAX_BUFFER); /* "clears" curr_line junk characters */
	
	/* runs on the file */
	while (fgets(curr_line, MAX_BUFFER, src) != NULL){
		
		line_num++; /* test.am file line counter */
		
		is_line_valid = 1;
		
		/* checks if the first run found an error in the current line */
		for (i = 0; i < (err_ln_size/sizeof(int)) && is_line_valid; i++){
		
			if (error_lines[i] == line_num)
				is_line_valid = 0;
		}
		
		if (!is_line_valid) /* if already found an error in the current line */
			continue;
		
		curr_line_ast = get_ast(curr_line);
		
		/* if the line is empty or a comment, skips it */
		if (curr_line_ast.ast_union_option == ast_union_empty_line ||
			curr_line_ast.ast_union_option == ast_union_comment_line)
			continue;
			
		/* if it is entry or extern line */
		if (curr_line_ast.ast_union_option == ast_union_dir &&
		 (curr_line_ast.ast_union_ins_dir.ast_dir.ast_union_dir_option == ast_union_dir_entry ||
		  curr_line_ast.ast_union_ins_dir.ast_dir.ast_union_dir_option == ast_union_dir_extern)){  
		  
			/* if it is entry line */
			if (curr_line_ast.ast_union_ins_dir.ast_dir.ast_union_dir_option == ast_union_dir_entry){
			
				/* finding the entry label in the table */
				curr_search_res = search_label(symbol_table, curr_line_ast.ast_union_ins_dir.ast_dir.dir.label);
			
				/* if the label is declared as entry and not defined in the file */
				if (curr_search_res -> comm == enum_comm_none){
				
					errprintf(src_name, line_num, "the label '%s' is declared as entry and is not defined in the file", curr_search_res -> label);
					is_line_valid = 0;
			
				}
		
			}
		
			/* if it is extern line */
			else if (curr_line_ast.ast_union_ins_dir.ast_dir.ast_union_dir_option == ast_union_dir_extern){
		
				/* runs on extern label array */
				for (i = 0; i < curr_line_ast.ast_union_ins_dir.ast_dir.dir.extern_labels_array.labels_count; i++){
				
					/* finding the entry label in the table */
					curr_search_res = search_label(symbol_table, curr_line_ast.ast_union_ins_dir.ast_dir.dir.extern_labels_array.labels[i]);
					
					if (curr_search_res -> label_value.value == NO_VALUE)
						warnprintf(src_name, line_num, "the label '%s' was declared as extern but not used in the file", curr_search_res -> label);
				
				
				}
		
		
			}
		}
		
		
		else { /* if it is an instruction or directive(string and data) line */
			
			/* encodes the instruction/directive into machine code */
			if (!encoder(code_im, ic_add, data_im, dc_add, &curr_line_ast, symbol_table, src_name, line_num))
				is_line_valid = 0;
		
		}
		
		
		if (is_valid && !is_line_valid) /* if current line is invalid the whole program is invalid */
			is_valid = 0;
			
		
	}
 			
 			
 			
	
 			
 			
	fclose(src);
	return is_valid;	
 			
}
