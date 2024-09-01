/*
 *	File: encoder.c
 *
 *	This program encodes assembly instructions and directives into machine code.
 *	It defines functions to encode instructions and directives based on the provided
 *	Abstract Syntax Tree (AST) structure.
 *
 *	author: Gal Levi
 *	version: 5.8.23
 */


#include "funcs_and_macs.h"
#include "ast.h"
#include "encoder.h"
#include "labels_BST.h"

/* macro definitions */

/* instruction op code = ast enum of instruction - 1 */
#define INS_OPCODE curr_line_ast -> ast_union_ins_dir.ast_ins.ins - 1 
#define MAX_IMM_NUM 511 /* maximum immediate value allowed (signed 10 bits) */
#define MIN_IMM_NUM -512 /* minimum immediate value allowed */
#define MAX_DATA_NUM 2047 /* maximum data value allowed (signed 12 bits) */
#define MIN_DATA_NUM -2048 /* minimum data value allowed */


/* exclusive functions prototype */
mem_code_word encode_first_word(ast *);
int insert_word_ins_with_operands(enum op_type_e, op_type_u, mem_code_word (*)[MAX_MEMORY_ASSUMPTION], 
									symbol_table_node *, int *, char *, int);
int encode_dir(mem_data_word (*)[MAX_MEMORY_ASSUMPTION], ast *, symbol_table_node *, int *, char *, int);
int encode_ins(mem_code_word (*)[MAX_MEMORY_ASSUMPTION], ast *, symbol_table_node *, int *, char *, int);




/*
 *	Main encoding function that determines whether to encode an instruction or a directive.
 *
 *	param code_im - Pointer to the code image memory buffer.
 *	param ic_add - Pointer to the instruction counter, used to keep track of the current address in the code image.
 *	param data_im - Pointer to the data image memory buffer.
 *	param dc_add - Pointer to the data counter, used to keep track of the current address in the data image.
 *	param curr_line_ast - The AST node representing the current line of code.
 *	param symbol_table - Pointer to the symbol table.
 *	param file_name - Name of the source file being processed.
 *	param line_num - Current line number in the source file.
 *	returns 1 if encoding is successful, 0 otherwise.
 */
int encoder(mem_code_word (*code_im)[MAX_MEMORY_ASSUMPTION], int *ic_add,
			 mem_data_word (*data_im)[MAX_MEMORY_ASSUMPTION], int *dc_add, 
 			ast *curr_line_ast, symbol_table_node *symbol_table, char *file_name, int line_num){

	/* if the line is instruction */
	if (curr_line_ast -> ast_union_option == ast_union_ins){

		return encode_ins(code_im, curr_line_ast, symbol_table, ic_add, file_name, line_num);

	}

	/* if the line is directive */
	else if (curr_line_ast -> ast_union_option == ast_union_dir){

		return encode_dir(data_im, curr_line_ast, symbol_table, dc_add, file_name, line_num);

	}
	
	return 1;
}




/*
 *	Encodes assembly instructions into machine code.
 *
 *	param code_im - Pointer to the code image memory buffer.
 *	param ic_add - Pointer to the instruction counter, used to keep track of the current address in the code image.
 *	param data_im - Pointer to the data image memory buffer.
 *	param dc_add - Pointer to the data counter, used to keep track of the current address in the data image.
 *	param curr_line_ast - The AST node representing the current line of code.
 *	param symbol_table - Pointer to the symbol table.
 *	param file_name - Name of the source file being processed.
 *	param line_num - Current line number in the source file.
 *	returns 1 if encoding is successful, 0 otherwise.
 */
int encode_ins(mem_code_word (*code_im)[MAX_MEMORY_ASSUMPTION], ast *curr_line_ast,
 				symbol_table_node *symbol_table, int *ic_add, char *file_name, int line_num){
		
	int i;
		
	/* encodes the first word and inserts it in the code image array*/
	(*code_im)[(*ic_add)++] = encode_first_word(curr_line_ast);

	/* 2 operands instructions case (mov, cmp, add, sub, lea) */
	if (curr_line_ast -> ast_union_ins_dir.ast_ins.ins >= ast_ins_mov &&
		curr_line_ast -> ast_union_ins_dir.ast_ins.ins <= ast_ins_lea){
				
		/* if the 2 operands are registers - the appropriate addressing method is register addressing */
		if (curr_line_ast -> ast_union_ins_dir.ast_ins.ast_ins_kind.ast_ins_two_op.ote[0] == ast_op_type_reg &&
			curr_line_ast -> ast_union_ins_dir.ast_ins.ast_ins_kind.ast_ins_two_op.ote[1] == ast_op_type_reg){
			
			/* inserts the register word into the code image array */
			(*code_im)[(*ic_add)].word_type = reg_word;
			(*code_im)[(*ic_add)].union_word.reg_add_word.are = abs_code;
			(*code_im)[(*ic_add)].union_word.reg_add_word.src_reg = curr_line_ast -> ast_union_ins_dir.ast_ins.ast_ins_kind.ast_ins_two_op.otu[0].reg;
			(*code_im)[(*ic_add)++].union_word.reg_add_word.des_reg = curr_line_ast -> ast_union_ins_dir.ast_ins.ast_ins_kind.ast_ins_two_op.otu[1].reg;
				
		}
			
			
		else { /* if there is at least 1 operand that is not a register */
			for (i = 0; i < 2; i++){ /* loop on the operands array */
				
				/* inserts the encoded operands into the image code array */
				if (!insert_word_ins_with_operands(curr_line_ast -> ast_union_ins_dir.ast_ins.ast_ins_kind.ast_ins_two_op.ote[i],
												 curr_line_ast -> ast_union_ins_dir.ast_ins.ast_ins_kind.ast_ins_two_op.otu[i],
												  code_im,  symbol_table,  ic_add,  file_name, line_num))
					return 0;
			}
		}
			
	} /* end of 2 operands case */
		
	/* one operand instructions case */
	else if (curr_line_ast -> ast_union_ins_dir.ast_ins.ins >= ast_ins_not &&
			curr_line_ast -> ast_union_ins_dir.ast_ins.ins <= ast_ins_jsr){
		
		if (!insert_word_ins_with_operands(curr_line_ast -> ast_union_ins_dir.ast_ins.ast_ins_kind.ast_ins_one_op.ote,
											curr_line_ast -> ast_union_ins_dir.ast_ins.ast_ins_kind.ast_ins_one_op.otu,
											code_im,  symbol_table,  ic_add,  file_name, line_num))
			return 0;
			
	}


	/* else - no operands instructions case is not exist (it requires only one word) */

	return 1;
}




/*
 *	Encodes assembly directives into machine code.
 *
 *	param data_im - Pointer to the data image memory buffer.
 *	param curr_line_ast - The AST node representing the current line of code.
 *	param symbol_table - Pointer to the symbol table.
 *	param dc_add - Pointer to the data counter, used to keep track of the current address in the data image.
 *	param file_name - Name of the source file being processed.
 *	param line_num - Current line number in the source file.
 *	returns 1 if encoding is successful, 0 otherwise.
 */
int encode_dir(mem_data_word (*data_im)[MAX_MEMORY_ASSUMPTION], ast *curr_line_ast,
 				symbol_table_node *symbol_table, int *dc_add, char *file_name, int line_num){
	
	int i;
	
	/* if it is string directive */
	if (curr_line_ast -> ast_union_ins_dir.ast_dir.ast_union_dir_option == ast_union_dir_string){
	
		/* inserts in the data image array the encoded string */
		for (i = 0; i < strlen(curr_line_ast -> ast_union_ins_dir.ast_dir.dir.string); i++){
			
			(*data_im)[(*dc_add)++].curr_data = (unsigned int)curr_line_ast -> ast_union_ins_dir.ast_dir.dir.string[i];
		
		
		}
		
		(*data_im)[(*dc_add)++].curr_data = 0; /* inserts the null terminator */
	
	} /* end of string case */
	
	
	else { /* if it is data directive */
		
		/* inserts in the data image array the encoded numbers */
		for (i = 0; i < curr_line_ast -> ast_union_ins_dir.ast_dir.dir.data_num_array.num_count; i++){
			
			if (curr_line_ast -> ast_union_ins_dir.ast_dir.dir.data_num_array.num_array[i] > MAX_DATA_NUM ||
				curr_line_ast -> ast_union_ins_dir.ast_dir.dir.data_num_array.num_array[i] < MIN_DATA_NUM){
			
				errprintf(file_name, line_num, "the number %d is out of range (data value range is -2048,...,2047)", curr_line_ast -> ast_union_ins_dir.ast_dir.dir.data_num_array.num_array[i]);
				return 0;
			
			}
			(*data_im)[(*dc_add)++].curr_data = curr_line_ast -> ast_union_ins_dir.ast_dir.dir.data_num_array.num_array[i];
		
		}
		
	
	} /* end of data case */
	
	return 1;
				 
}



/*
 *	Inserts the instruction word with its operands into the code image.
 *
 *	param ote - Operand type enumeration for the current operand.
 *	param otu - Operand type union for the current operand.
 *	param code_im - Pointer to the code image memory buffer.
 *	param symbol_table - Pointer to the symbol table.
 *	param ic_add - Pointer to the instruction counter, used to keep track of the current address in the code image.
 *	param file_name - Name of the source file being processed.
 *	param line_num - Current line number in the source file.
 *	returns 1 if the operand insertion is successful, 0 otherwise.
 */
int insert_word_ins_with_operands(enum op_type_e ote, op_type_u otu, 
								mem_code_word (*code_im)[MAX_MEMORY_ASSUMPTION],
								 symbol_table_node *symbol_table, int *ic_add, char *file_name,
								  int line_num){

	symbol_table_node *curr_search_res;
		
	/* if current operand is register (if so, the second operand is surely not a register) */
	if (ote == ast_op_type_reg){
		
		/* inserts the encoded register word into the image code array */
		(*code_im)[(*ic_add)].word_type = reg_word;
		(*code_im)[(*ic_add)].union_word.reg_add_word.are = abs_code;
		(*code_im)[(*ic_add)++].union_word.reg_add_word.src_reg = otu.reg;

			
	}
			
	/* if current operand is a label - the appropriate addressing method is direct */
	else if (ote == ast_op_type_label){
		
		(*code_im)[(*ic_add)].word_type = imm_dir_word;
		
		/* if the label exist in the table */
		if ((curr_search_res = search_label(symbol_table, otu.label))){
			
			/* if the type is relocatable or defined entry */
			if (curr_search_res -> type == enum_rel || 
			   (curr_search_res -> type == enum_ent && curr_search_res -> comm != enum_comm_none)){
				(*code_im)[(*ic_add)].union_word.imm_direct_add_word.are = rel_code;
				(*code_im)[(*ic_add)].union_word.imm_direct_add_word.operand = curr_search_res -> label_value.value + MEMORY_ASSUMPTION;
			}
			
			/* if the type is external */
			else if (curr_search_res -> type == enum_extl){  
				(*code_im)[(*ic_add)].union_word.imm_direct_add_word.are = ext_code;
				(*code_im)[(*ic_add)].union_word.imm_direct_add_word.operand = 0;
			}
			
			else { /* if the type is none/undefined entry the label has not been defined at all */
				
				errprintf(file_name, line_num, "the label '%s' has not been defined anywhere", otu.label);
				return 0;
			}
					
					
		(*ic_add)++;
		}
			
	}
			
	/* if current operand is a number - the appropriate addressing method is immediate */
	else {
		
		(*code_im)[(*ic_add)].word_type = imm_dir_word;
		(*code_im)[(*ic_add)].union_word.imm_direct_add_word.are = abs_code;
				
		/* if the immediate value exceeds the limit of 10 bits */
		if (otu.imm > MAX_IMM_NUM || otu.imm < MIN_IMM_NUM){
				
			errprintf(file_name, line_num, "the number %d is out of range (immediate value range is -512,...,511)", otu.imm);
			return 0;
				
		}
				
		/* if the number is in the range */
		else
			(*code_im)[(*ic_add)++].union_word.imm_direct_add_word.operand = otu.imm;
					
	}
	
	return 1;


}


/*
 *	Encodes the first word of an instruction.
 *
 *	param curr_line_ast - The AST node representing the current instruction.
 *	returns the encoded first word as a mem_code_word structure.
 */
mem_code_word encode_first_word(ast *curr_line_ast){


	mem_code_word res;
	
	res.union_word.first_word.are = abs_code; /* first word is an absolute code */
	
	res.word_type = first_word; /* updates the type of word */
	
	res.union_word.first_word.op_code = INS_OPCODE;
	
	/* two opernads instructions case */
	if (curr_line_ast -> ast_union_ins_dir.ast_ins.ins >= ast_ins_mov &&
		curr_line_ast -> ast_union_ins_dir.ast_ins.ins <= ast_ins_lea){
			
		res.union_word.first_word.src_op_add = curr_line_ast -> ast_union_ins_dir.ast_ins.ast_ins_kind.ast_ins_two_op.ote[0];
		res.union_word.first_word.des_op_add = curr_line_ast -> ast_union_ins_dir.ast_ins.ast_ins_kind.ast_ins_two_op.ote[1];
	}
	
	/* one operand instructions case */
	else if (curr_line_ast -> ast_union_ins_dir.ast_ins.ins >= ast_ins_not &&
			curr_line_ast -> ast_union_ins_dir.ast_ins.ins <= ast_ins_jsr){
			
			res.union_word.first_word.src_op_add = 0;
			res.union_word.first_word.des_op_add = curr_line_ast -> ast_union_ins_dir.ast_ins.ast_ins_kind.ast_ins_one_op.ote;
			
	}
	
	/* no operands instructions case */
	else { 
		
		res.union_word.first_word.src_op_add = 0;
		res.union_word.first_word.des_op_add = 0;
	
	}
	
	
	return res;
	



}


