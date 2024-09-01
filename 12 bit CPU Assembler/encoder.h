/*
 *	File: encoder.h
 *
 *	Defines data structures and constants related to encoding assembly instructions and directives into 
 *	machine code.
 *
 *	author: Gal Levi
 *	version: 5.8.23
 */


enum { /* A,R,E codes */

	abs_code, /* in binary: 00 */
	ext_code, /* in binary: 01 */
	rel_code /* in binary: 10 */
	
};

typedef struct { /* memory code word */
	
	enum {
		first_word = 1, /* if it is first word */
		imm_dir_word, /* if it is immediate/direct addressing word */
		reg_word /* if it is register addressing word */
	} word_type;
	
	union { /* divides 3 types of memory words */
	
		struct { /* first word of block */
		
			unsigned int are: 2; /* A,R,E */
			unsigned int des_op_add: 3;
			unsigned int op_code: 4;
			unsigned int src_op_add: 3;

		} first_word;



		struct { /* immediate or direct addressing word */
		
			unsigned int are: 2;
			unsigned int operand: 10;

		} imm_direct_add_word;


	 	struct { /* register addressing word */
	 	
			unsigned int are: 2;
			unsigned int des_reg: 5;
			unsigned int src_reg: 5;

		}reg_add_word;
		
	} union_word;
	
} mem_code_word;


typedef struct { /* string or data memory word */

	unsigned int curr_data: 12;

} mem_data_word;








