/*
 *	File: labels_bst.h
 *
 *  This header file defines data structures and function prototypes related to managing a symbol table 
 *	using a binary search tree (BST).
 *  It includes structure definitions for symbol table nodes and enums for label types and command types.
 *  
 *  author: Gal Levi
 *  version: 5.8.23
 */


#include "funcs_and_macs.h"
#define NO_VALUE -1 /* for external and entry lables that have not yet been defined */

enum enum_type{
	enum_rel = 1, /* if it is a relocatable type */
	enum_ent, /* if the label is entry */
	enum_extl, /* if the label is external */
	enum_type_none /* if the label is a parameter and has not yet been defined */
};

enum enum_comm{
	enum_dir = 1, /* directive label (e.g., string or data directives) */
	enum_ins,
	enum_comm_none /* if the label has not yet been defined (after entry/extern declaration) */
};

/* define the symbol table node structure */
typedef struct symbol_table_node {
    char label[MAX_LABEL_SIZE];
    union {
    	int value;
    	struct {
    		int extern_values[MAX_MEMORY_ASSUMPTION];
    		int values_count;
    	}label_extern_value; /* if the label is external */
    	struct {
    		int param_values[MAX_MEMORY_ASSUMPTION];
    		int values_count;
    	}label_param_value; /* if the label is parameter */
    }label_value;
    enum enum_type type;
    enum enum_comm comm;
    struct symbol_table_node *left;
    struct symbol_table_node *right;
} symbol_table_node;

/* functions prototype */
void free_symbol_table(symbol_table_node **);
symbol_table_node *search_label(symbol_table_node *, char *);
symbol_table_node *insert_label(symbol_table_node *, char *, int, enum enum_type, enum enum_comm);
symbol_table_node *create_label_node(char *, int, enum enum_type, enum enum_comm);
void update_label_type(symbol_table_node *, enum enum_type);
void increase_labels_value_by_comm(symbol_table_node *, int, enum enum_comm);
void print_extern_labels_to_stream(FILE *, symbol_table_node *);
void print_entry_labels_to_stream(FILE *, symbol_table_node *);
void export_entry_and_extern_labels(char *, symbol_table_node *);
int is_there_used_extern(symbol_table_node *);
int is_there_entry(symbol_table_node *);



