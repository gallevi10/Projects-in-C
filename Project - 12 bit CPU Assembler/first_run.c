/*
 *	File: first_run.c
 *
 *  This source file contains the implementation of the first_run function,
 *	which is responsible for processing the input assembly file during the first run of the assembler.
 *	It handles label definitions, instruction and directive processing, and performs various error checks.
 *  
 *  author: Gal Levi
 *  version: 5.8.23
 */


#include "macro_list.h"
#include "labels_BST.h"
#include "ast.h"

/* macro definitions */
#define L_INS_TWO_REGS 2 /* number of memory words for instructions with two registers */
#define L_INS_NO_PARAMS 1 /* number of memory words for instructions with no parameters */
#define ENLARGE_SIZE *err_ln_size_add += sizeof(int) /* size increase for error line number array */



/*
 *  Process the first run of the assembler on an extended input assembly file(.am file).
 *
 *  This function processes the input assembly file during the first run of the assembler.
 *  It handles label definitions, instruction and directive processing, and performs various error checks.
 *  The function updates the symbol table with labels and their associated information and calculates
 *  the values of the instructions (IC) and data (DC) counters.
 *
 *  param file_name - The name of the input assembly file.
 *  param label_root_add - Pointer to the root of the symbol table.
 *  param head_add - Pointer to the head of the macro linked list.
 *  param ic_add - Pointer to the instruction counter.
 *  param dc_add - Pointer to the data counter.
 *  param err_ln_add - Pointer to an array storing error line numbers.
 *  param err_ln_size_add - Pointer to the size of the errors line array.
 *  returns 1 if the first run processing is successful, -1 if there is a memory overflow and 0 otherwise.
 */
int first_run(char *file_name, symbol_table_node **label_root_add, macro_node **head_add,
			 int *ic_add, int *dc_add, int **err_ln_add, int *err_ln_size_add){

	int line_num = 0, is_valid = 1, is_line_valid = 1, i;
	FILE *src;
	char src_name[MAX_BUFFER], curr_line[MAX_BUFFER]; /* MAX_BUFFER = 1024 */
	ast curr_line_ast;
	symbol_table_node *curr_search_res;
	
	(*err_ln_add) = (int *)calloc(*err_ln_size_add, 0);
	
	if (!(*err_ln_add)){
		errprintf(NO_FILE_ERROR, NO_LINE_ERROR, "Memory allocation failed - first run");
        exit(1);
	}
	
	sprintf(src_name, "%s.am", file_name);
	
	if (!(src = fopen(src_name, "r"))){
	
		errprintf(src_name, NO_LINE_ERROR, "cannot open file - first run");
		return 0;
		
	}
	
	memset(curr_line, '\0', MAX_BUFFER); /* "clears" curr_line junk characters */
	
	/* runs on the file */
	while (fgets(curr_line, MAX_BUFFER, src) != NULL){
		
		line_num++; /* test.am file line counter */
		
		is_line_valid = 1;
		
		curr_line_ast = get_ast(curr_line);
		
		
		/* if the line is empty or a comment, skips it */
		if (curr_line_ast.ast_union_option == ast_union_empty_line ||
			curr_line_ast.ast_union_option == ast_union_comment_line)
			continue;
			
		/* errors check */
		
		/* if there is a label definition in current line */
		if (curr_line_ast.label_def_flag){
		
			/* if the current line label is a macro */
			if (is_macro_exist(*head_add, curr_line_ast.label)){
			
				errprintf(src_name, line_num, "the label '%s' is already a defined macro - label definition", curr_line_ast.label);
				is_line_valid = 0;
			}
			/* if the current line label is already in the table */
			else if ((curr_search_res = search_label(*label_root_add, curr_line_ast.label))){
			
				if (curr_search_res -> comm == enum_comm_none){ /* if the label command is none (external or entry label) */
					if (curr_search_res -> type == enum_extl){ /* if the label is external */
						
						errprintf(src_name, line_num, "the label '%s' is already declared as external and could not be defined as local", curr_line_ast.label);
						is_line_valid = 0;
					
					}
				}
				else { /* if the label command is instruction or directive */
					errprintf(src_name, line_num, "the label '%s' is already defined", curr_line_ast.label);
					is_line_valid = 0;
				}
			}	
			
		}
		
		/* if there is already an error in ast checks */
		if (is_line_valid && curr_line_ast.ast_union_option == ast_union_error){ 
			
			errprintf(src_name, line_num, curr_line_ast.ast_error);
			is_line_valid = 0;
		}
		
		/* that's all the errors for now */
			
		/* instructions case */
		if (is_line_valid && curr_line_ast.ast_union_option == ast_union_ins){
			
			if (curr_line_ast.label_def_flag)
				/* iserts the label in the table */
				*label_root_add = insert_label(*label_root_add, curr_line_ast.label, *ic_add, enum_rel, enum_ins);
			
				
			/* 2 parameters instructions case (mov, cmp, add, sub, lea) */
			if (curr_line_ast.ast_union_ins_dir.ast_ins.ins >= ast_ins_mov &&
				curr_line_ast.ast_union_ins_dir.ast_ins.ins <= ast_ins_lea){
				
				/* if the 2 parameters are registers */
				if (curr_line_ast.ast_union_ins_dir.ast_ins.ast_ins_kind.ast_ins_two_op.ote[0] == ast_op_type_reg &&
					curr_line_ast.ast_union_ins_dir.ast_ins.ast_ins_kind.ast_ins_two_op.ote[1] == ast_op_type_reg)
					*ic_add += L_INS_TWO_REGS; /* two registers are sharing one memory word */
				
				else { 
					(*ic_add)++; /* adds to count the first word(the instruction command) */
					
					for (i = 0; i < 2; i++){
						/* if the parameters are labels */
						if (curr_line_ast.ast_union_ins_dir.ast_ins.ast_ins_kind.ast_ins_two_op.ote[i] == ast_op_type_label)
							*label_root_add = insert_label(*label_root_add, curr_line_ast.ast_union_ins_dir.ast_ins.ast_ins_kind.ast_ins_two_op.otu[i].label, *ic_add, enum_type_none, enum_comm_none);
						
						(*ic_add)++;
					}
				}
				
			}
				
			/* one parameter instructions case */
			else if (curr_line_ast.ast_union_ins_dir.ast_ins.ins >= ast_ins_not &&
				curr_line_ast.ast_union_ins_dir.ast_ins.ins <= ast_ins_jsr){
				(*ic_add)++; /* adds to count the first word(the instruction command */
				
				if (curr_line_ast.ast_union_ins_dir.ast_ins.ast_ins_kind.ast_ins_one_op.ote == ast_op_type_label)
					*label_root_add = insert_label(*label_root_add, curr_line_ast.ast_union_ins_dir.ast_ins.ast_ins_kind.ast_ins_one_op.otu.label, *ic_add, enum_type_none, enum_comm_none);
						
				(*ic_add)++;
				
			}
				
			else /* not parameters instructions case */
				
				*ic_add += L_INS_NO_PARAMS;
				
				
		} /* end of instructions case */
		
		
		/* directives case */
		else if (is_line_valid && curr_line_ast.ast_union_option == ast_union_dir){
			
			/* if it is string or data directives */
			if (curr_line_ast.ast_union_ins_dir.ast_dir.ast_union_dir_option == ast_union_dir_string ||
				curr_line_ast.ast_union_ins_dir.ast_dir.ast_union_dir_option == ast_union_dir_data){
				
				if (curr_line_ast.label_def_flag)
					*label_root_add = insert_label(*label_root_add, curr_line_ast.label, *dc_add, enum_rel, enum_dir);
				
				
				/* if it is string directive */
				if (curr_line_ast.ast_union_ins_dir.ast_dir.ast_union_dir_option == ast_union_dir_string){
					
					/* +1 for '\0' */
					*dc_add += (strlen(curr_line_ast.ast_union_ins_dir.ast_dir.dir.string) + 1);
				}
				
				/* if it is data directive */
				else
					*dc_add += curr_line_ast.ast_union_ins_dir.ast_dir.dir.data_num_array.num_count;
		
			}
			
			/* if it is an entry declaration */
			else if (curr_line_ast.ast_union_ins_dir.ast_dir.ast_union_dir_option == ast_union_dir_entry){
			
				if (curr_line_ast.label_def_flag) /* if there is a label defiition in entry definition line */
					warnprintf(src_name, line_num, "the label '%s' has no meaning - label definition on entry declaration", curr_line_ast.label);
				
				/* if the entry label is already exist in the table */
				if ((curr_search_res = search_label(*label_root_add, curr_line_ast.ast_union_ins_dir.ast_dir.dir.label))){
					
					/* if the exist label type is external */
					if (curr_search_res -> type == enum_extl){
					
						errprintf(src_name, line_num, "the label '%s' is already declared as extern and could not be redeclared as entry", curr_search_res -> label);
						is_line_valid = 0;
					}
					
					else if (curr_search_res -> type == enum_rel) /* if the type is relocatable - updates the type to entry*/
						
						update_label_type(curr_search_res, enum_ent);
				
				
				}
				
				/* else - the label is not in the table or not defined yet or already declared as entry */
				if (is_line_valid)
					*label_root_add = insert_label(*label_root_add, curr_line_ast.ast_union_ins_dir.ast_dir.dir.label, NO_VALUE, enum_ent, enum_comm_none);
					
					
			}
			else { /* if it is extern declaration */
			
				if (curr_line_ast.label_def_flag) /* if there is a label defiition in entry definition line */
					warnprintf(src_name, line_num, "the label '%s' has no meaning - label definition on extern declaration", curr_line_ast.label);
			
				/* runs on extern label array */
				for (i = 0; is_line_valid && i < curr_line_ast.ast_union_ins_dir.ast_dir.dir.extern_labels_array.labels_count; i++){
				
					/* if the extern label is already exist in the table */
					if ((curr_search_res = search_label(*label_root_add, curr_line_ast.ast_union_ins_dir.ast_dir.dir.extern_labels_array.labels[i]))){
					
						/* if the exist label type is entry */
						if (curr_search_res -> type == enum_ent){
					
							errprintf(src_name, line_num, "the label '%s' is already declared as entry and could not be redeclared as external", curr_search_res -> label);
							is_line_valid = 0;
					
						}
					
						else if (curr_search_res -> type == enum_rel) { /* if the type is rellocation */
						
							errprintf(src_name, line_num, "the label '%s' is already defined as local and could not be declared as external", curr_search_res -> label);
							is_line_valid = 0;
						}
				
				
					}
					
					/* else - the label is not in the table or not defined yet or already declared as external */
					if (is_line_valid)
						*label_root_add = insert_label(*label_root_add, curr_line_ast.ast_union_ins_dir.ast_dir.dir.extern_labels_array.labels[i], NO_VALUE, enum_extl, enum_comm_none);
				
				
				
				
				} /* end of for */
				
			
			}
		
		
		
		
		
		} /* end of directives case */
		
		if (!is_line_valid){
			
			(*err_ln_add) = realloc(*err_ln_add, ENLARGE_SIZE); /* ENLARGE_SIZE = err_ln_size += sizeof(int) */
			
			(*err_ln_add)[((*err_ln_size_add)/sizeof(int)) - 1] = line_num;
			
			if (is_valid)
				is_valid = 0;
		
		}
		
		
		memset(curr_line, '\0', MAX_BUFFER);
	}
	
	
	if ((*ic_add + *dc_add) > MAX_MEMORY_ASSUMPTION){
	
		errprintf(src_name, NO_LINE_ERROR, "memory overflow - the maximum memory you can use is %d memory words but you exceeded it and reached %d.", MAX_MEMORY_ASSUMPTION, (*ic_add + *dc_add));
		is_valid = -1; /* flag indicating there is a memory error */
	
	}
	
	
	/* adding ic for all directive labels */
	if (is_valid == 1)
		increase_labels_value_by_comm(*label_root_add, *ic_add, enum_dir);
	
	

	fclose(src);
	return is_valid;
}



