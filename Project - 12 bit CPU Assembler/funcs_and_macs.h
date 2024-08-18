/*
 *	File: funcs_and_macs.h
 *
 *  This header file defines various utility functions and macros used throughout the assembler program. 
 *	These functions handle string manipulation,
 *  whitespace handling, error and warning printing, and other common tasks.
 *
 *  author: Gal Levi
 *  version: 5.8.23
 */


#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

/* macro definitions */

/* increases idx to the first occurrence of non white character in the given string */
#define JUMP_TO_NEXT_NON_WHITE(str_ptr, idx) \
	while (isspace(str_ptr[idx])){ \
		idx++; \
	}

/* increases idx to the first occurrence of white character in the given string */
#define JUMP_TO_FIRST_WHITE(str_ptr, idx) \
	while (str_ptr[idx] != '\0' && !isspace(str_ptr[idx])){ \
		idx++; \
	}

/* decreases idx to the first occurrence of white character in the given string */
#define JUMP_BACKWARDS_TO_FIRST_NON_WHITE(str_ptr, idx) \
	while (idx > 0 && isspace(str_ptr[idx])){ \
		idx--; \
	}
	
#define MAX_LINE 82 /* maximum length for an assembly line (+1 for null and +1 for new line) */
#define MAX_BUFFER 1024 /* maximum length for an input buffer */
#define MAX_MEMORY_ASSUMPTION 924 /* maximum allowable memory (1024 - 100) */
#define MAX_LABEL_SIZE 32 /* maximum label size (+1 for null) */
#define NO_LINE_ERROR -1 /* indicator for no line number */
#define NO_FILE_ERROR "NO FILE" /* indicator for no file name */
#define MEMORY_ASSUMPTION 100 /* assignment memory stack assumption (starts from 100) */


void remove_white(char *);
int is_white(char *);
int count_char(char *, char);
int is_white_n(char *, int);
int is_sep_by_white(char *);
int is_num(char *);
int is_reserved_word(char *);
int is_num_and_punc(char *);
void remove_white_from_borders(char *);
int is_valid_lm(char *, int);
int is_comment(char *);
void errprintf(const char [], const int , const char *, ...);
void warnprintf(const char [], const int, const char *, ...);
int check_length(char *);


