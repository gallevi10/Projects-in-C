/*
 *	File: macro_list.h
 *
 *  This header file defines data structures and function prototypes related to managing macros and their 
 *	lines in the assembler program.
 *  It includes structure definitions for macro nodes and their lines, along with function prototypes for 
 *	creating and manipulating them.
 *  
 *  author: Gal Levi
 *  version: 5.8.23
 */

#include "funcs_and_macs.h"


/* defines the structures for macro and its lines of code */
typedef struct l_node {

    char line[MAX_LINE];
    struct l_node *next;
    struct l_node *prev;
    
}line_node;

typedef struct m_node {

    char macro[MAX_LINE];
    struct m_node *next;
    struct m_node *prev;
    line_node *head_line;
    line_node *tail_line;
    
}macro_node;


/* functions prototype */
macro_node *create_macro(char[]);
void insert_macro(macro_node **, macro_node **, char[]);
line_node *create_line(macro_node **, char[]);
void insert_line(macro_node **, char[]);
macro_node *is_macro(macro_node **, char []);
void free_macro_list(macro_node **);
void delete_macro_lines(macro_node **);
void macro_to_string(char *, macro_node *);
int is_macro_exist(macro_node *, char []);


