/*
 *  File: ast.c
 *
 *  This file contains the implementation of functions that handle the construction
 *  and manipulation of an Abstract Syntax Tree (AST) used in assembly code processing.
 *  The AST represents the parsed assembly code structure, including instructions,
 *  data directives, and labels. It provides functions for creating, updating, and
 *  validating the AST based on the assembly code input.
 *
 *  The AST structure stores various types of information, such as instruction details,
 *  labels, data directives, errors, and more. The functions defined in this file help
 *  ensure proper syntax checking, validation of input, and storage of parsed information
 *  in the AST nodes.
 *
 *  The AST serves as an intermediate representation of the assembly code, enabling further
 *  processing, analysis, and eventual translation into machine code.
 *
 *	author: Gal Levi
 *	version: 5.8.23
 */


#include "ast.h"

/* macro definitions */
#define DIRS_NUM 4 /* number of available directives */
#define INSS_NUM 16 /* number of available instructions */
#define INS_LEN 3 /* length of usual instruction */
#define STOP_LEN 4 /* length of 'stop' instruction */
#define TWO_PARAMS_SUPPOSED_COMMAS 1 /* expected number of commas in an instruction with two parameters */
/* expected number of parameters in an instruction with two parameters */
#define TWO_PARAMS_SUPPOSED_PARAMS_NUM 2
#define REGS_NUM 8 /* number of available registers */
/* gets the current instruction */
#define CURR_INS INSS_SEP_BY_GRPS[new_ast -> ast_union_ins_dir.ast_ins.ins - 1]
/* checks if the current line corresponds to a directive */
#define IS_DIRECTIVE(i) memcmp(line_ptr, DIRS[i], DIRS_LEN[i]) == 0 && isspace(*(line_ptr + DIRS_LEN[i]))


/* exclusive functions prototype */
char *is_there_label(char *, ast *);
void is_dir(char *, ast *);
int check_comma(char *, ast *, char *);
int check_end(char *, ast *, char *, int);
int check_data(char [][MAX_LINE], int, ast *);
int is_valid_label(char *, ast *, char []);
void is_ins(char *, ast *);
int check_ins_ops_syn(char [][MAX_LINE], int , ast *, const char *);
int check_extern(char [][MAX_LINE], int , ast *);
void skip_to_params(char **, int );
void divide(char *, char (*)[][MAX_LINE], int *);


/*
 *	This function parses a line of code and returns an Abstract Syntax Tree (AST) representing the line.
 *   
 *	param line - The line of code to be parsed
 *	returns The AST representing the parsed line
 */
ast get_ast(char *line){
	
	ast new_ast = {0}; /* the function output */
	char *line_ptr, *after_label, is_com;
	
	if (!(check_length(line))){ /* checks if the length of the line is valid */
				
		new_ast.ast_union_option = ast_union_error;
		strcpy(new_ast.ast_error, "line overflow - length of line is up to 80 characters");
		return new_ast;
	}

	if ((is_com = is_comment(line))){ /* checks if the current line is a comment */
	
		switch (is_com){
		
			case 1: new_ast.ast_union_option = ast_union_comment_line;
			break;
			case 2: new_ast.ast_union_option = ast_union_error;
					strcpy(new_ast.ast_error, "invalid comment line - the character ';' is not the first character of the line");
			break;
		
		}
		return new_ast;
	
	}
	
	if (is_white(line)){ /* if the current line is an empty one(consists only of white characters) */
	
		new_ast.ast_union_option = ast_union_empty_line;
		return new_ast;
	}
	
	/* checks if the line starts with a label definition */
	after_label = is_there_label(line, &new_ast);
	if (new_ast.ast_union_option == ast_union_error){ /* found an error in label analyzing */
		return new_ast;
	}
	
	else if (after_label != NULL) /* if there is a valid label definition at the beginning of the line */
		line_ptr = after_label; /* line_ptr = the text after the label definition (after ':') */
	
	else
		line_ptr = line; /* if there is not an error or label definition */
	
	is_dir(line_ptr, &new_ast); /* checks if the line is a directive line */
	
	/* if found a new error or the line is a directive sentence */
	if (new_ast.ast_union_option == ast_union_error || 
		new_ast.ast_union_option == ast_union_dir){ 
		return new_ast;
	}
	
	/* checks if the line is an instruction sentence */
	is_ins(line_ptr, &new_ast);
	
	
	return new_ast;	/* returns the updated ast */




}


/*
 *	This function checks if a label is present at the beginning of the line.
 *	If a label is found, it validates the label and updates the AST accordingly.
 *   
 *	param line - The line of code to check for a label
 *	param new_ast - Pointer to the current state of the AST
 *	returns - Pointer to the character after the label if found, else NULL
 */
char *is_there_label(char *line, ast *new_ast){
	
	int is_valid, length;
	char *line_ptr, *label;
	
	
	if ((line_ptr = strchr(line, ':')) != NULL){ /* there is a label definition */
		
		length = line_ptr - line; /* calculates the length of the label (including white characters) */
		
		/* if there is at least 1 white character before ':' */
		if (length > 1 && isspace(*(line_ptr-1))){ 
		
			new_ast -> ast_union_option = ast_union_error;
			strcpy(new_ast -> ast_error, "':' is supposed to come right after the label name - label definition");
			return NULL;
			
		}
		
		label = (char *)malloc((sizeof(char) * length) + 1); /* maximum potentially length and +1 for null */
		
		if (!label){
			errprintf(NO_FILE_ERROR, NO_LINE_ERROR, "memory allocation failed - label definition");
			exit(1);
		}
		
		memcpy(label, line, length); /* inserts in label the label name */
		label[length] = '\0';
		
		/* is_valid = 1 - the label is valid */
		is_valid = is_valid_label(label, new_ast, "label definition");
			
		
		
		if (is_valid){ /* if the label itself is valid */
		
			strcpy(new_ast -> label, label);
			new_ast -> label_def_flag = 1;
			
			if (is_white(line_ptr + 1)){ /* last check - if it is an empty definition */
		
				new_ast -> ast_union_option = ast_union_error;
				strcpy(new_ast -> ast_error, "empty label definition");
				is_valid = 0;
		
			}
		}
		/* if everything is ok and the label definition is valid returns line_ptr + 1
	 	   else - if found an error returns NULL */
		free(label);
		return is_valid ? line_ptr + 1 : NULL;
	}
	
	/* if there is not a label definition returns NULL */
	return NULL;
}


/*
 *	This function checks if a line contains a directive and processes it.
 *	It updates the AST with the appropriate directive information.
 *   
 *	param line - The line of code to check for a directive
 *	param new_ast - Pointer to the current state of the AST
 */
void is_dir(char *line, ast *new_ast){

	int idx = 0, chr_cnt, length, partitions_cnt, i;
	char *line_ptr, partitions[MAX_LINE][MAX_LINE];
	const char *DIRS[] = {"string", "data", "entry", "extern"};
	const int DIRS_LEN[] = {6, 4, 5, 6}; /* directives lengths in order (DIRS array order) */
	
	
	JUMP_TO_NEXT_NON_WHITE(line, idx)
	line_ptr = line + idx; /* line_ptr now supposed to point on the dot of the directives (if directive was entered) */
	
	/* checks if the directives entered without a dot */
	for (i = 0; i < DIRS_NUM; i++){
		if (memcmp(line_ptr, DIRS[i], DIRS_LEN[i]) == 0){
			new_ast -> ast_union_option = ast_union_error;
			sprintf(new_ast -> ast_error, "'%s' directive is missing the dot", DIRS[i]);
			return;
		}
	}
	if ((*line_ptr == '.')){ /* there is a directive dot */
		
		new_ast -> ast_union_option = ast_union_dir; /* updates the ast union option to directive */
		line_ptr++; /* skips to the directive itself */
		if ((IS_DIRECTIVE(0))){ /* if it is string directive */
			 
			 /* updates the ast directive option to string */
			 new_ast -> ast_union_ins_dir.ast_dir.ast_union_dir_option = ast_union_dir_string;
			 
			 line_ptr += DIRS_LEN[0]; /* skips the directive name */
			 if ((chr_cnt = count_char(line_ptr, '"')) < 2){ /* counts the occurrence of quotation marks */
			 				
			 	new_ast -> ast_union_option = ast_union_error; /* updates the ast union option to error */
			 	switch (chr_cnt){
			 	
			 		case 0: if (is_white(line_ptr)) /* if the parameter is blank */
			 					strcpy(new_ast -> ast_error, "missing parameter - string definition");
			 				else /* if the parameter is not in the requested format at all */
			 					strcpy(new_ast -> ast_error, "invalid parameter - string definition");
			 		break;
			 		case 1: strcpy(new_ast -> ast_error, "missing quotation mark - string definition");
			 		break;
			 	}
			 
			 	
			 }
			 
			 /* strchr(line_ptr, '"') - line_ptr = the length of the text before the string parameter */
			 else if (!is_white_n(line_ptr, strchr(line_ptr, '"') - line_ptr)){
			 	
			 	new_ast -> ast_union_option = ast_union_error;
			 	strcpy(new_ast -> ast_error, "extraneous text before parameter - string definition");
			 
			 }
			 
			 
			 else{ /* valid string */
			 	
			 	
			 	line_ptr = strchr(line_ptr, '"'); /* line_ptr now points to the first '"' in line */
			 	line_ptr++; /* skips to the string(parameter) itself */
			 	length = strrchr(line_ptr, '"') - line_ptr; /* computes the length of the entered string */
			 	/* inserts the string in the ast */
			 	memcpy(new_ast -> ast_union_ins_dir.ast_dir.dir.string, line_ptr, length);
			 	new_ast -> ast_union_ins_dir.ast_dir.dir.string[length] = '\0'; /* adds null to end of entered string */
			 	
			 	/* checking the end of string directive */
			 	check_end(line_ptr + length + 1, new_ast, "string definition", 1);
		
			 }
			 	
		} /* end of string case */
		
		
		else if ((IS_DIRECTIVE(1))){ /* if it is data directive */
			 
			 /* updates the ast directive option to string */
			 new_ast -> ast_union_ins_dir.ast_dir.ast_union_dir_option = ast_union_dir_data;
			 
			 /* skips to the parameters of data */
			 skip_to_params(&line_ptr, DIRS_LEN[1]);
			 
			 /* one parameter case */
			 if (count_char(line_ptr, ',') == 0){
			 
			 	if (*line_ptr == '\0') { /* if there is nothing after the directive name */
			 	
			 		new_ast -> ast_union_option = ast_union_error;
					strcpy(new_ast -> ast_error, "missing parameter - data definition");
					return;
			 
			 	}
			 	
			 	strcpy(partitions[0], line_ptr);
				if(!check_data(partitions, 1, new_ast)) /* checks if the parameter is invalid */
					return;
			 		
			 	/* if the parameter is valid */
					
			 	new_ast -> ast_union_ins_dir.ast_dir.dir.data_num_array.num_array[0] = atoi(partitions[0]);
			 	new_ast -> ast_union_ins_dir.ast_dir.dir.data_num_array.num_count = 1;
			 	
			 }
			 
			 else { /* two or more parameters case */
			 
				if (!check_comma(line_ptr, new_ast, "data definition"))
			 		return;
			 	
			 	/* inserts to partitions array the parameters */
			 	divide(line_ptr, &partitions, &partitions_cnt);
			 
			 	if(!check_data(partitions, partitions_cnt, new_ast)) /* checks if there is an invalid parameter */
					return;
			 
			 	/* if the parameters are valid */
			 	for (i = 0; i < partitions_cnt; i++) /* inserts the numbers in the ast */
			 		new_ast -> ast_union_ins_dir.ast_dir.dir.data_num_array.num_array[i] = atoi(partitions[i]);
			 
			 	/* updates the count of numbers of data in the ast */
			 	new_ast -> ast_union_ins_dir.ast_dir.dir.data_num_array.num_count = partitions_cnt;
			 }
			 
			 	
			 	
			 
			 
		} /* end of data case */
		
		else if ((IS_DIRECTIVE(2))){ /* if it is entry directive */
			/* assumes there can be only 1 parameter */

			/* updates the ast directive option to entry */
			new_ast -> ast_union_ins_dir.ast_dir.ast_union_dir_option = ast_union_dir_entry;
			
			/* skips to the parameter of entry */
			skip_to_params(&line_ptr, DIRS_LEN[2]);
			
			if (*line_ptr == '\0'){
				
				new_ast -> ast_union_option = ast_union_error;
				strcpy(new_ast -> ast_error, "missing parameter - entry declaration");
				return;
				
			}
			
			if (strchr(line_ptr, ',')){
			
				new_ast -> ast_union_option = ast_union_error;
				strcpy(new_ast -> ast_error, "comma is not allowed in entry declaration");
				return;
			
			}
			
			/* checking the labal */
			if (!is_valid_label(line_ptr, new_ast, "entry parameter"))
				return;
		
			/* the label is valid */
			strcpy(new_ast -> ast_union_ins_dir.ast_dir.dir.label, line_ptr);
		
		} /* end of entry case */
		
		else if ((IS_DIRECTIVE(3))){ /* if it is extern directive */
			/* assumes there can be more than 1 parameter */
			
			/* updates the ast directive option to extern */
			new_ast -> ast_union_ins_dir.ast_dir.ast_union_dir_option = ast_union_dir_extern;
			
			/* skips to the parameters of extern */
			skip_to_params(&line_ptr, DIRS_LEN[3]);
			 
			/* one parameter case */
			if (count_char(line_ptr, ',') == 0){
			
				if (*line_ptr == '\0') {
			 	
			 		new_ast -> ast_union_option = ast_union_error;
					strcpy(new_ast -> ast_error, "missing parameter - extern declaration");
					return;
			 
			 	}
			 	
			 	strcpy(partitions[0], line_ptr);
				if(!check_extern(partitions, 1, new_ast)) /* checks if the parameter is invalid */
					return;
			 		
			 	/* the parameter is valid */
					
			 	strcpy(new_ast -> ast_union_ins_dir.ast_dir.dir.extern_labels_array.labels[0], partitions[0]);
			 	new_ast -> ast_union_ins_dir.ast_dir.dir.extern_labels_array.labels_count = 1;
			}
			
			else { /* two or more parameters */
				
				if (!check_comma(line_ptr, new_ast, "extern declaration"))
			 		return;
			 
			 	divide(line_ptr, &partitions, &partitions_cnt);
			 	
			 	if(!check_extern(partitions, partitions_cnt, new_ast)) /* checks if there is an invalid parameter */
					return;
			 
			 	/* if the parameters are valid */
			 	for (i = 0; i < partitions_cnt; i++) /* inserts the numbers to the ast */
			 		strcpy(new_ast -> ast_union_ins_dir.ast_dir.dir.extern_labels_array.labels[i], partitions[i]);
			 
			 	/* updates the count of numbers of data in the ast */
			 	new_ast -> ast_union_ins_dir.ast_dir.dir.extern_labels_array.labels_count = partitions_cnt;
			
			}
		
		} /* end of extern case */
		
		else { /* if it is an undefined directive */
		
			idx = 0;
			JUMP_TO_FIRST_WHITE(line_ptr, idx)
			line_ptr[idx] = '\0';
			/* line_ptr is now the undefined directive(without the possible parameters) */
			
			new_ast -> ast_union_option = ast_union_error;
			if (*line_ptr == '\0') /* if the line has only the dot and missing the directive name */
				strcpy(new_ast -> ast_error, "missing directive");
			else
				sprintf(new_ast -> ast_error, "'.%s' is an undefined directive", line_ptr);
		
		}
		
	}
	
	/* else -  it is not a directive */

}


/*
 *	This function checks if a line contains an instruction and processes it.
 *	It updates the AST with the appropriate instruction information.
 *   
 *	param line - The line of code to check for an instruction
 *	param new_ast - Pointer to the current state of the AST
 */
void is_ins(char *line, ast *new_ast){
	
	int idx = 0, i, partitions_cnt;
	char *line_ptr, partitions[MAX_LINE][MAX_LINE], error_cont[MAX_LINE];
	
	/* uses the order defined in the enum in h file,
	the first 5 instrucion receives 2 parameters, the other 9 instructions receives 1 parameter and
	the last 2 instructions does not receives any parameters */
	const char *INSS_SEP_BY_GRPS[] = {"mov", "cmp", "add", "sub", "lea", "not", "clr", "inc", "dec",
						 				"jmp", "bne", "red", "prn", "jsr", "rts", "stop"};
	 							
	
	JUMP_TO_NEXT_NON_WHITE(line, idx)
	line_ptr = line + idx; 
	/* line_ptr now supposed to point to the first character of the instruction (if it is an instruction) */

	/* instruction identifying */
	for (i = ast_ins_mov; i <= INSS_NUM && new_ast -> ast_union_option == 0; i++){ /* ast_ins_mov = 1 */
	
		if (i == INSS_NUM){ /* if it is stop instruction */
			
			if (strncmp(line_ptr, INSS_SEP_BY_GRPS[i-1], STOP_LEN) == 0){ /* STOP_LEN = 4 */
				if (isspace(*(line_ptr + STOP_LEN)) || *(line_ptr + STOP_LEN) == '\0'){
					new_ast -> ast_union_option = ast_union_ins; /* updates the ast union option to instruction */
					new_ast -> ast_union_ins_dir.ast_ins.ins = i; /* i is currently ast_ins_stop = INSS_NUM = 16 */
				}
			
			
			
			}
		}
		
		else if (strncmp(line_ptr, INSS_SEP_BY_GRPS[i-1], INS_LEN) == 0){ /* INS_LEN = 3 (instruction length) */			
			if (isspace(*(line_ptr + INS_LEN)) || *(line_ptr + INS_LEN) == '\0'){
			
				new_ast -> ast_union_option = ast_union_ins; /* updates the ast union option to instruction */
				new_ast -> ast_union_ins_dir.ast_ins.ins = i; /* i is currently the suitable number in the defined enum */
			}

			
		}
			
	}
	
	
	
	
	/* if the line is an instruction line */
	if (new_ast -> ast_union_option == ast_union_ins) { 
		
		/* skips to the operands of current instruction */
		if (new_ast -> ast_union_ins_dir.ast_ins.ins != ast_ins_stop) /* if the instruction is not 'stop' */
			skip_to_params(&line_ptr, INS_LEN);
		else /* if the instruction is 'stop' */
			skip_to_params(&line_ptr, STOP_LEN);
		
		/* two operands instructions case */
		if (new_ast -> ast_union_ins_dir.ast_ins.ins >= ast_ins_mov &&
			new_ast -> ast_union_ins_dir.ast_ins.ins <= ast_ins_lea) { /* enum instructions 1 - 5 */
			
			
			if (*line_ptr == '\0'){ /* if not entered opernads at all */
				
				new_ast -> ast_union_option = ast_union_error;
				sprintf(new_ast -> ast_error, "missing operands - '%s' instruction", CURR_INS);
				return;
				
			}
			
			sprintf(error_cont, "'%s' instruction", CURR_INS);
			
			/* checks the commas in the line */
			if (!check_comma(line_ptr, new_ast, error_cont)) 
				return;

			divide(line_ptr, &partitions, &partitions_cnt);
			
			if (partitions_cnt > TWO_PARAMS_SUPPOSED_PARAMS_NUM /* = 2 */){
			
				new_ast -> ast_union_option = ast_union_error;
				sprintf(new_ast -> ast_error, "'%s' instruction recieved too many operands", CURR_INS);
				return;
			
			}
			
			/* checks instruction operands syntax */
			if (!check_ins_ops_syn(partitions, partitions_cnt, new_ast, CURR_INS))
				return;
			
			/* the operands syntax is ok, now is left to check the addressing method for each instruction */
			
			/* lea source operand case - the others supports all methods */
			if (new_ast -> ast_union_ins_dir.ast_ins.ins == ast_ins_lea && 
				new_ast -> ast_union_ins_dir.ast_ins.ast_ins_kind.ast_ins_two_op.ote[0]
				!= ast_op_type_label){
				
				new_ast -> ast_union_option = ast_union_error;
				strcpy(new_ast -> ast_error, "'lea' source operand supports only direct addressing");
				return;
				
				
			}
			
			/* destination operand case (all the 2 operands instructions except cmp) */
			if (new_ast -> ast_union_ins_dir.ast_ins.ins != ast_ins_cmp && 
				new_ast -> ast_union_ins_dir.ast_ins.ast_ins_kind.ast_ins_two_op.ote[1]
				== ast_op_type_imm){
				
				new_ast -> ast_union_option = ast_union_error;
				sprintf(new_ast -> ast_error, "'%s' destination operand does not support immediate addressing", CURR_INS);
				
			}
			
		} /* end of 2 operands case */
		
		/* one operand instructions case */
		else if (new_ast -> ast_union_ins_dir.ast_ins.ins >= ast_ins_not &&
				new_ast -> ast_union_ins_dir.ast_ins.ins <= ast_ins_jsr) { /* enum instructions 6 - 14 */

			
			if (*line_ptr == '\0'){ /* if not entered operand at all */
				
				new_ast -> ast_union_option = ast_union_error;
				sprintf(new_ast -> ast_error, "missing operand - '%s' instruction", CURR_INS);
				return;
				
			}
			
			if (strchr(line_ptr, ',')){ /* if comma was entered */
				
				new_ast -> ast_union_option = ast_union_error;
				sprintf(new_ast -> ast_error, "comma is not allowed in '%s' definition", CURR_INS);
				return;
			
			}
			
			/* getting the operand ready for checking */
			strcpy(partitions[0], line_ptr);
			partitions_cnt = 1;
			
			/* checks the operand syntax */
			if (!check_ins_ops_syn(partitions, partitions_cnt, new_ast, CURR_INS))
				return;
				
			/* checks the addressing method of the param (all the 1 param instructions except prn) */
			if (new_ast -> ast_union_ins_dir.ast_ins.ins != ast_ins_prn && 
				new_ast -> ast_union_ins_dir.ast_ins.ast_ins_kind.ast_ins_one_op.ote == ast_op_type_imm){
				
				new_ast -> ast_union_option = ast_union_error;
				sprintf(new_ast -> ast_error, "'%s' destination operand does not support immediate addressing", CURR_INS);
				
				
			}
		} /* end of 1 operand case */
			
		/* no operands instructions case */
		else if (new_ast -> ast_union_ins_dir.ast_ins.ins >= ast_ins_rts &&
				new_ast -> ast_union_ins_dir.ast_ins.ins <= ast_ins_stop){
			
			sprintf(error_cont, "'%s' definition", CURR_INS);
			check_end(line_ptr, new_ast, error_cont, 1);
			
		}
			
			
			

	}
	
	else { /* if it is an unknown instruction */
		
		idx = 0;
		JUMP_TO_FIRST_WHITE(line_ptr, idx)
		line_ptr[idx] = '\0';
		/* line_ptr is now the undefined instruction(without the possible operands) */
		
		new_ast -> ast_union_option = ast_union_error;
		sprintf(new_ast -> ast_error, "'%s' is an undefined instruction", line_ptr);
	
	
	
	}
	
	
	
}


/*
 *	Checks the presence and validity of commas in the parameter list.
 *
 *	param str - The parameter list to check for commas
 *	param new_ast - Pointer to the current state of the AST
 *	param error_cont - The relevant continuation for the error message
 *	returns 1 if commas are valid, 0 otherwise
 */
int check_comma(char *str, ast *new_ast, char *error_cont) {
	
	int commas;
	char cpy[MAX_LINE];
	
	if (*str == ','){
		new_ast -> ast_union_option = ast_union_error;
		sprintf(new_ast -> ast_error, "illegal comma - %s", error_cont);
		return 0;
	}
		
	if (new_ast -> ast_union_option == ast_union_ins){ /* if it is an instruction */
		
		if ((commas = count_char(str, ',')) < TWO_PARAMS_SUPPOSED_COMMAS){
		
			new_ast -> ast_union_option = ast_union_error;
			sprintf(new_ast -> ast_error, "missing comma - %s", error_cont);
			return 0;
		
		}
		
		/* if there is 1 comma but no operand */
		if ((commas == TWO_PARAMS_SUPPOSED_COMMAS && is_white(strrchr(str, ',') + 1))) {
		
			new_ast -> ast_union_option = ast_union_error;
			sprintf(new_ast -> ast_error, "missing operand - %s", error_cont);
			return 0;
		}

	}
	
	strcpy(cpy, str);
	remove_white(cpy);
	if (strstr(cpy, ",,")){ /* checks if the line has multiple consecutive commas between parameters */
		
		new_ast -> ast_union_option = ast_union_error;
		sprintf(new_ast -> ast_error, "multiple consecutive commas - %s", error_cont);
		return 0;
	
	}
	
	/* checks the characters after last comma */
	if (!check_end(strrchr(str, ',') + 1, new_ast, error_cont, 0))
		return 0;


	return 1;

}


/*
 *	Checks for the correct termination of a parameter list.
 *
 *	param str - The parameter list to check
 *	param new_ast - Pointer to the current state of the AST
 *	param error_cont - The relevant continuation for the error message
 *	param supposed_white - Flag indicating if white space is expected
 *	returns 1 if the parameter list is correctly terminated, 0 otherwise
 */
int check_end(char *str, ast *new_ast, char *error_cont, int supposed_white){
	
	/* checking the end of str */
	if (((new_ast -> ast_union_option == ast_union_ins) && (supposed_white == 0 && is_white(str)))||
		(supposed_white == 1 && !is_white(str))){ /* if it is a command with limited parameters */
		
		new_ast -> ast_union_option = ast_union_error;
		sprintf(new_ast -> ast_error, "extraneous text after end of %s", error_cont);
		return 0;
		
	}
	else if (supposed_white == 0 && is_white(str)){
	
		new_ast -> ast_union_option = ast_union_error;
		sprintf(new_ast -> ast_error, "missing parameter - %s", error_cont);
		return 0;
	
	}
		
	
	return 1;

}


/*
 *	Checks the validity of parameters in data directive.
 *
 *	param partitions - An array of parameters to check
 *	param cnt - The number of parameters
 *	param new_ast - Pointer to the current state of the AST
 *	returns 1 if parameters are valid, 0 otherwise
 */
int check_data(char partitions[][MAX_LINE], int cnt, ast *new_ast){

	int i;
	
	for (i = 0; i < cnt; i++){
		
		remove_white_from_borders(partitions[i]); /* removes white characters from borders */
		
		if (is_sep_by_white(partitions[i])){
			new_ast -> ast_union_option = ast_union_error;
			sprintf(new_ast -> ast_error, "invalid data parameter - the parameter '%s' is seperated by white characters", partitions[i]);
			return 0;
		}
				
		if (!is_num(partitions[i])){
			
			new_ast -> ast_union_option = ast_union_error;
			sprintf(new_ast -> ast_error, "invalid data parameter - '%s' is not an integer", partitions[i]);
			return 0;
				
		}
		
		
	}	
	
	
	return 1;



}



/*
 *	Checks the validity of a label according to specified criteria.
 *
 *	param label - The label to be checked
 *	param new_ast - Pointer to the current state of the AST
 *	param error_cont - The relevant continuation for the error message
 *	returns 1 if the label is valid, 0 otherwise
 */
int is_valid_label(char *label, ast *new_ast, char error_cont[]){
		
	switch(is_valid_lm(label, 1)){
		
		case 2: new_ast -> ast_union_option = ast_union_error;
				sprintf(new_ast -> ast_error, "the label '%s' is an empty word - %s", label, error_cont);
		break;
		
		case 3: new_ast -> ast_union_option = ast_union_error;
				sprintf(new_ast -> ast_error, "the label '%s' starts with an unalphabetic character - %s", label, error_cont);
		break;
		
		case 4: new_ast -> ast_union_option = ast_union_error;
				sprintf(new_ast -> ast_error, "the label '%s' is seperated by white characters - %s", label,  error_cont);
		break;
		
		case 5: new_ast -> ast_union_option = ast_union_error;
				sprintf(new_ast -> ast_error, "the label '%s' is too long - %s", label,  error_cont);
		break;
		
		case 6: new_ast -> ast_union_option = ast_union_error;
				sprintf(new_ast -> ast_error, "the label '%s' is a reserved word - %s", label, error_cont);
		break;
		
		case 7: new_ast -> ast_union_option = ast_union_error;
				sprintf(new_ast -> ast_error,"the label '%s' contains a non-alphanumeric character - %s", label, error_cont);
		break;
		
		default: return 1; /* if valid returns 1 */
	
	}

	
	
	return 0; /* if invalid returns 0 */	  
								  

}

/*
 *	Checks the syntax of instruction parameters and populates the AST with operand information.
 *
 *	param partitions - Array of instruction parameters
 *	param cnt - The number of parameters
 *	param new_ast - Pointer to the current state of the AST
 *	param curr_ins - The current instruction being processed
 *	returns 1 if parameter syntax is valid, 0 otherwise
 */
int check_ins_ops_syn(char partitions[][MAX_LINE], int cnt, ast *new_ast, const char *curr_ins){

	int i, j;
	char error_cont[MAX_LINE];
	const char *REGS[] = {"r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7"};
	
	for (i = 0; i < cnt; i++){ /* cnt is 2 at most */
	
		remove_white_from_borders(partitions[i]);
		
		if (is_sep_by_white(partitions[i])){
			new_ast -> ast_union_option = ast_union_error;
			sprintf(new_ast -> ast_error, "invalid '%s' instruction operand - the operand '%s' is seperated by white characters", curr_ins, partitions[i]);
			return 0;
		}
			
		
		/* register case */
		
		if (*partitions[i] == '@'){
			for (j = 0; j < REGS_NUM; j++){
				
				if (strcmp(partitions[i] + 1, REGS[j]) == 0){ /* partitions[i] + 1 is after @ */
					
					if (cnt == 2){ /* if it is a 2 params instructions */
						/* inserts to the ast the new found register */
						new_ast -> ast_union_ins_dir.ast_ins.ast_ins_kind.ast_ins_two_op.ote[i] = ast_op_type_reg;
						new_ast -> ast_union_ins_dir.ast_ins.ast_ins_kind.ast_ins_two_op.otu[i].reg = atoi((partitions[i] + 2)); 	/* partitions[i] + 2 is the register number */
					}
					else { /* if it is a 1 param instructions */
						new_ast -> ast_union_ins_dir.ast_ins.ast_ins_kind.ast_ins_one_op.ote = ast_op_type_reg;
						new_ast -> ast_union_ins_dir.ast_ins.ast_ins_kind.ast_ins_one_op.otu.reg = atoi((partitions[i] + 2));
					}
					break;
					
				
				}
			
			
			}
			if (j == REGS_NUM){ /* if it is an unknown register */
				new_ast -> ast_union_option = ast_union_error;
				sprintf(new_ast -> ast_error, "invalid '%s' instruction operand - the opernad '%s' is an unknown register", curr_ins, partitions[i]);
				return 0;
			}
		} /* end of register case */
		
		
		
		/* immediate value case (number) */
		else if (is_num_and_punc(partitions[i])){
			
			if (is_num(partitions[i])){ /* it is a valid immediate value */
			
				if (cnt == 2){
					new_ast -> ast_union_ins_dir.ast_ins.ast_ins_kind.ast_ins_two_op.ote[i] = ast_op_type_imm;
					new_ast -> ast_union_ins_dir.ast_ins.ast_ins_kind.ast_ins_two_op.otu[i].imm = atoi(partitions[i]);
				}
				else{
					new_ast -> ast_union_ins_dir.ast_ins.ast_ins_kind.ast_ins_one_op.ote = ast_op_type_imm;
					new_ast -> ast_union_ins_dir.ast_ins.ast_ins_kind.ast_ins_one_op.otu.imm = atoi(partitions[i]);
				}
			}
			else { /* it is not an integer number */
			
				new_ast -> ast_union_option = ast_union_error;
				sprintf(new_ast -> ast_error, "invalid '%s' instruction operand - the opernad '%s' is not an integer", curr_ins, partitions[i]);
				return 0;
				
			}
			
				
		} /* end of immediate case */
		
		else { /* it is probably a label */
			
			sprintf(error_cont, "'%s' instruction operand", curr_ins);
			
			if (!is_valid_label(partitions[i], new_ast, error_cont))
				return 0;
			
			/* if it is a valid label */
			
			/* inserts to the ast the found label */
			if (cnt == 2){
				new_ast -> ast_union_ins_dir.ast_ins.ast_ins_kind.ast_ins_two_op.ote[i] = ast_op_type_label;
				strcpy(new_ast -> ast_union_ins_dir.ast_ins.ast_ins_kind.ast_ins_two_op.otu[i].label, partitions[i]);
			}
			else {
				new_ast -> ast_union_ins_dir.ast_ins.ast_ins_kind.ast_ins_one_op.ote = ast_op_type_label;
				strcpy(new_ast -> ast_union_ins_dir.ast_ins.ast_ins_kind.ast_ins_one_op.otu.label, partitions[i]);
			}
			
			
		} /* end of label case */
			
	} /* end of for */
	

	return 1;

}


/*
 *	Checks the validity of extern labels according to specified criteria.
 *
 *	param partitions - Array of extern labels
 *	param cnt - The number of extern labels
 *	param new_ast - Pointer to the current state of the AST
 *	returns 1 if extern labels are valid, 0 otherwise
 */
int check_extern(char partitions[MAX_LINE][MAX_LINE], int cnt, ast *new_ast){

	int i;
	
	for (i = 0; i < cnt; i++){
		
		remove_white_from_borders(partitions[i]);
				
		if (!is_valid_label(partitions[i], new_ast, "extern labels"))
			return 0;
				
	}	
	
	
	return 1;



}



/*
 *	Skips to the parameter section of a line.
 *
 *	param line_ptr_add - Pointer to the line to be modified (updated to point to parameter section)
 *	param comm_len - The length of the command to be skipped
 */
void skip_to_params(char **line_ptr_add, int comm_len){
	
	int idx = 0;
	
	(*line_ptr_add) += comm_len; /* skips the command */
	JUMP_TO_NEXT_NON_WHITE((*line_ptr_add), idx)
	*line_ptr_add = (*line_ptr_add) + idx;
	/* line_ptr now points to the first character of the data parameters */


}


/*
 *	Divides a line into partitions using a delimiter and stores them in an array.
 *
 *	param line_ptr - Pointer to the line to be divided
 *	param partitions_add - Pointer to the array to store partitions
 *	param cnt_add - Pointer to the variable to store the number of partitions
 */
void divide(char *line_ptr, char (*partitions_add)[][MAX_LINE], int *cnt_add){

	char *token = strtok(line_ptr, ",");
	*cnt_add = 0;
	do {
		strcpy((*partitions_add)[*cnt_add], token);
		token = strtok(NULL, ",");
		(*cnt_add)++;
	}while (token != NULL);
	
}




