/* 
 * File: pre_assembler.c
 *
 * This file handles the pre-assembler phase, macro processing, and preparation for the first run.
 * It also includes utility functions for handling lines, macros, and checking line lengths.
 * This program processes source files, identifies macros,
 * checks syntax validity, and generates intermediate files for further processing.
 *
 * author: Gal Levi
 * version: 5.8.23
 */


#include "macro_list.h"

/* macro definitions */
#define BEFORE_ENDMCRO_AND_MCRO line_ptr - curr_line /* the text before endmcro/mcro statement */
#define AFTER_ENDMCRO line_ptr + 7 /* the text after endmcro statement */
#define AFTER_MCRO line_ptr + 4 /* the text after mcro statement */
#define ENLARGE_SIZE draft_size += sizeof(char) * MAX_LINE /* enlarges the size of draft(in realloc) */
#define IS_ENDMCRO line_ptr = strstr(curr_line, "endmcro") /* used to identify endmcro line */
#define IS_MCRO line_ptr = strstr(curr_line, "mcro") /* used to identify mcro line */
/* the text after the name of macro which found in current line */
#define AFTER_MACRO_TEXT strstr(curr_line, curr_macro -> macro) + strlen(curr_macro -> macro)
/* the length of the text before the name of macro which found in current line */
#define BEFORE_MACRO_TEXT_LEN strstr(curr_line, curr_macro -> macro) - curr_line




/*
 * Handles the pre-assembler phase, which includes macro processing and preparation for the first run.
 *
 * param file_name - The name of the source file without extension.
 * param head_add - Pointer to the head of the macro list.
 * param tail_add - Pointer to the tail of the macro list.
 * Returns 1 if the pre-assembler phase completes successfully, otherwise returns 0.
 */
int pre_assembler(char file_name[], macro_node **head_add, macro_node **tail_add){
	
	macro_node *curr_macro;
	FILE *src, *des;
	/* MAX_BUFFER = 1024 */
	char src_name[MAX_BUFFER], des_name[MAX_BUFFER], curr_line[MAX_BUFFER] , temp[MAX_BUFFER], *line_ptr;
	int mcro_flag = 0, line_num = 0, draft_size = 0, is_valid = 1, is_macro_valid;
	char *draft = (char *)calloc(draft_size, sizeof(char)); /* stores the result */
	
	if (!draft){ /* if allocation was failed */
	
		errprintf(NO_FILE_ERROR, NO_LINE_ERROR, "memory allocation failed - pre assembler");
		exit(1);
	
	}
	
	sprintf(src_name, "%s.as", file_name); /* src_name = <file_name>.as */
	sprintf(des_name, "%s.am", file_name);
	
	if (!(src = fopen(src_name, "r"))){ /* if the .as file failed to open */
	
		errprintf(src_name, NO_LINE_ERROR, "cannot open file - pre assembler");
		free(draft);
		return 0;
		
	}
	
	
	/* inserts lines from .as file to curr_line until EOF */
	while (fgets(curr_line, MAX_BUFFER, src) != NULL){ 
		
		line_num++; /* test.as file line counter */
		
		if ((IS_ENDMCRO)){ /* if current line has endmcro statement */
		 	

		    /* checks the corners of endmcro statement */
		    memset(temp, '\0', MAX_LINE);
			strncpy(temp, curr_line, BEFORE_ENDMCRO_AND_MCRO);
			/* temp comtains the text before endmcro statement */
			
			if (!is_white(temp) || !is_white(AFTER_ENDMCRO)){
				
				errprintf(src_name, line_num, "additional characters are not allowed in endmcro statement");
				is_valid = 0; /* pre assembler failed */

			}
			/* checks the length of endmcro statement line (length of white characters) */
			else if (!(check_length(curr_line))){
		 		
		 		errprintf(src_name, line_num, "endmcro statement line overflow - length of line is up to 80 characters"); 
		 		is_valid = 0; 
		 		continue;
		 		
		 	}
			else { /* the line of endmcro is valid */
				if (mcro_flag == 1)
					mcro_flag = 0;
			}
		        
	    }
		
		/* if it is mcro definition line */
        else if (mcro_flag == 0 && (IS_MCRO)){
        	
        	/* checks the length of mcro statement line */
        	if (!(check_length(curr_line))){
		 		
		 		errprintf(src_name, line_num, "mcro statement line overflow - length of line is up to 80 characters");
		 		is_valid = 0;
		 		continue;
		 		
		 	}
            
            memset(temp, '\0', MAX_LINE); /* "clears" junk characters */ 
			strncpy(temp, curr_line, BEFORE_ENDMCRO_AND_MCRO);
			/* temp now comtains the text before mcro statement */
			
			line_ptr = AFTER_MCRO; /* line_ptr now points to the first character after mcro statement */
			
			/* checking mcro statement left corner */
			if (!is_white(temp)){
				
				errprintf(src_name, line_num, "additional characters are not allowed in mcro statement");
				is_valid = 0;
				continue;
			}
			
			/* checks macro syntax */
			if ((is_macro_valid = is_valid_lm(line_ptr, 0)) != 1){
			
				switch(is_macro_valid){
					
					
					case 2:	errprintf(src_name, line_num, "the macro '%s' is an empty word", line_ptr);
					break;
					
					case 3: errprintf(src_name, line_num, "the macro '%s' starts with an unalphabetic character", line_ptr);
					break;
		
					case 4: errprintf(src_name, line_num, "the macro '%s' is seperated by white characters", line_ptr);
					break;
		
					case 6: errprintf(src_name, line_num, "the macro '%s' is a reserved word", line_ptr);
					break;
		
					case 7: errprintf(src_name, line_num, "the macro '%s' contains a non-alphanumeric character", line_ptr);
					break;

	
				}
				
				is_valid = 0; 
			}
			else if (is_macro_exist(*head_add, line_ptr)){ /* checks if the macro is already defined */
			
				errprintf(src_name, line_num, "the macro '%s' is already defined", line_ptr);
				is_valid = 0;
			}
			else { /* the macro is valid */
				mcro_flag = 1; /* turns on macro definition flag */
				insert_macro(head_add, tail_add, line_ptr);
			}

        }
		
		/* there is not endmcro in this line, hence, the line needs to be stored in the current macro */
		else if (mcro_flag == 1 && is_valid)
		    insert_line(tail_add, curr_line);
		    
		    
		/* outside of macro definition */
		else if (mcro_flag == 0 && is_valid){
			
			/* ENLARGE_SIZE = draft_size += sizeof(char) * MAX_LINE(82) */
			draft = (char *)realloc(draft, ENLARGE_SIZE); 

			if (!draft){
			
				errprintf(NO_FILE_ERROR, NO_LINE_ERROR, "memory allocation failed - pre assembler");
				exit(1);
			
			}
			
			/* clears junk characters every reallocation */
			memset(draft + (sizeof(char) * (draft_size - MAX_LINE)), '\0', MAX_LINE);
		
			
			/* if the line contains an already exits macro and it is not a label definition,
			since this is a label error the first run should find it */
			if((curr_macro = is_macro(head_add, curr_line)) &&
			 !(strchr(AFTER_MACRO_TEXT, ':') && count_char(curr_line, ':') == 1)){ 
				
				/* assumes there is only a macro name in the line (as indicated on page 32) */
				if (!is_white_n(curr_line, BEFORE_MACRO_TEXT_LEN) || !is_white(AFTER_MACRO_TEXT)){
					
					errprintf(src_name, line_num, "additional characters are not allowed in macro line");
					is_valid = 0;
				
				}
				/* checks the length of mcro statement line */
				else if (!(check_length(curr_line))){
		 		
		 		errprintf(src_name, line_num, "macro line overflow - length of line is up to 80 characters");
		 		is_valid = 0;
		 		
		 	}
				else { /* it is a valid macro line */
				
					macro_to_string(draft, curr_macro); /* inserts in darft the macro lines */
					
				}
			}
			else
				strcat(draft, curr_line); /* inserts in draft the current line */
			
		}
		
	} /* end of while */
	
	if (is_valid){ /* if pre assembler did not failed */
	
		if (!(des = fopen(des_name, "w"))){ /* creates .am file */
	
			errprintf(des_name, NO_LINE_ERROR, "cannot open file");
			return 0;
		
		}
		
		else {  /* writes the draft in the .am file */
			fprintf(des, "%s", draft);
			fclose(des);
		}
	}
	
	else { /* found an error */
		
		if (*(head_add) != NULL)
			free_macro_list(head_add); 
	
	}
	
	
	
	free(draft);
	
	
	fclose(src);
	
	return is_valid;
	
	

}






