/* 
 * File: assembler.c
 *
 * This file contains the main logic for the assembler program. It handles the processing of input files,
 * including pre-assembler phase, label handling, and generation of output files.
 *
 * author: Gal Levi
 * version: 5.8.23
 */


#include "assembler.h"



/* 
 * main function of the program
 *
 * param argc - Number of command-line arguments.
 * param argv - Array of command-line arguments.
 * returns 0 on successful execution.
 */
int main(int argc, char *argv[]){

	int i, is_valid, is_pre_valid, err_ln_size, ic, dc, *error_lines = NULL,
		is_first_valid, is_second_valid; 
	macro_node *head_macro = NULL, *tail_macro = NULL; /* initializes macro data structure */
	symbol_table_node *label_root = NULL; /* initializes labels table data structure */
	mem_code_word code_im[MAX_MEMORY_ASSUMPTION] = {0}; /* code image */
	mem_data_word data_im[MAX_MEMORY_ASSUMPTION] = {0}; /* data image */
	
	/* if no file was entered in command line */
	if (argc < 2){
	
		errprintf(NO_FILE_ERROR, NO_LINE_ERROR, "no file was entered (expected format: %s file file ...)", argv[0]);
		return 0;
	}
	
	/* loop that runs over all the entered files */
	for (i = 1; i < argc; i++){
	
		/* resets validation flags */
		is_valid = 1;
		err_ln_size = 0;
		
		/* pre assembler run */
		if ((is_pre_valid = pre_assembler(CURR_FILE_NAME, &head_macro, &tail_macro))){ 
			delete_macro_lines(&head_macro); /* now we need only the macro names */
		}
		else  /* if an error was found in the pre assembler */
			continue;
		
		
		/* resets code and data counters */
		ic = 0;
		dc = 0;
		
		/* first run */
		if ((is_first_valid = first_run(CURR_FILE_NAME, &label_root, &head_macro, &ic, &dc, &error_lines, 
							 			&err_ln_size)) != 1)
			is_valid = 0;
		
		/* frees the macro list (we don't need it from now on) */
		if (head_macro != NULL)	
			free_macro_list(&head_macro);
			
		/* resets code and data counters again for the second run */
		ic = 0;
		dc = 0;
		
		/* second run */
		if (is_first_valid != -1){ /* if is_first_valid does not indicate a memory error */ 
			if (!(is_second_valid = second_run(&(code_im), &ic, &(data_im), &dc, label_root, 
												CURR_FILE_NAME, error_lines, err_ln_size)))
				is_valid = 0;
		}
		
		/* frees error_lines */
		if (error_lines != NULL)
			free(error_lines);
		
		/* creates and writes .ent, .ext, .ob files (if the program is valid so far) */
		if (is_valid) {
			
			/* creates and writes .ent, .ext files */
			export_entry_and_extern_labels(CURR_FILE_NAME, label_root);
			
			/* creates and writes .ob file (while converting to BASE64) */
			export_code_and_data_in_base64(CURR_FILE_NAME, &(code_im), ic, &(data_im), dc);
		
		}

		/* frees labels table */
		if (label_root != NULL)
			free_symbol_table(&label_root);
			
	}
	

	return 0;



}


