/*
 *	File: ast.h
 *
 *	This file contains the settings and data structures for the abstract syntax tree used in ast.c.
 *
 *	author: Gal Levi
 *	version: 5.8.23
*/

#include "funcs_and_macs.h"

#define MAX_ERROR_LEN 201
#define MAX_STRING_LEN 81
#define MAX_NUMBER_SIZE 81

enum instructions { /* op code of each instruction is (<enum> - 1) */

    /* 2 params commands */
    ast_ins_mov = 1, /* starts from 1 to prevent misunderstanding with initialization value */
    ast_ins_cmp,
    ast_ins_add,
    ast_ins_sub,
    ast_ins_lea,
    
    /* 1 param commands */
    ast_ins_not,
    ast_ins_clr,
    ast_ins_inc,
    ast_ins_dec,
    ast_ins_jmp,
    ast_ins_bne,
    ast_ins_red,
    ast_ins_prn,
    ast_ins_jsr,
    
    /* no params commands */
    ast_ins_rts,
    ast_ins_stop
};

enum op_type_e {
    ast_op_type_imm = 1, /* the values are according to the addressing methods codes */ 
    ast_op_type_label = 3,
    ast_op_type_reg = 5
};

typedef union {
    char label[MAX_LABEL_SIZE];
    char reg; /* register */
    int imm; /* immediate value */
}op_type_u; /* operand type */

typedef struct {
    char label[MAX_LABEL_SIZE]; /* the label of the label definition */
    int label_def_flag; /* flag to ensure there is a label definition in current ast */
    char ast_error[MAX_ERROR_LEN];
    enum {
        ast_union_error = 1,
        ast_union_ins, /* instruction */
        ast_union_dir,  /* directive */
        ast_union_empty_line, /* empty line */
        ast_union_comment_line /* comment line */
    }ast_union_option;
    union {
        
        struct { /* directive struct */
            enum {
                ast_union_dir_string = 1,
                ast_union_dir_data,
                ast_union_dir_entry,
                ast_union_dir_extern
            }ast_union_dir_option;
            union {
                char string[MAX_STRING_LEN]; /* if current directive is string */
                struct { /* if current directive is data */
                     int num_array[MAX_NUMBER_SIZE];
                     int num_count;
                }data_num_array;
                char label[MAX_LABEL_SIZE]; /* if current directive is entry */
                struct { /* if current directive is extern */
                	char labels[MAX_NUMBER_SIZE][MAX_LABEL_SIZE]; 
                	int labels_count;
                }extern_labels_array;
            }dir;
        }ast_dir;
        
        struct { /* instructions struct */
            enum instructions ins;
            union {
                struct { /* two parameters instructions */
                    enum op_type_e ote[2]; /* each index contains the enum of the operand */
                    op_type_u otu[2]; /* each index contains the operand */
                }ast_ins_two_op;

                struct { /* one parameter instructions */
                    enum op_type_e ote; /* contains the code of the operand (addressing method) */
                    op_type_u otu; /* contains the operand (can be label, register, immediate value) */
                }ast_ins_one_op;
            }ast_ins_kind;
        }ast_ins;

    }ast_union_ins_dir;
} ast;

/* functions prototype */
ast get_ast(char *);


