/*
 *	File: data_structures.c
 *
 *  This file contains the implementation of various data structures used in the assembler program.
 *  It defines functions for managing macro lists and symbol tables, including insertion, searching,
 *  and memory deallocation.
 *  
 *  The file includes functionality to handle macro definitions and lines, as well as symbol table
 *  nodes for labels. It implements macros for deep matching of macro names and for conveniently
 *  accessing the current available cell in the extern_values array for a label node.
 *
 *  author: Gal Levi
 *  version: 5.8.23
 */

#include "macro_list.h"
#include "labels_BST.h"

/* macro definitions */

/* checks if the current line contains the macro name with proper whitespace checks, ensuring deep matching */
#define MACRO_NAME_DEEP_CHECK isspace(*(line_ptr + strlen(curr -> macro))) && (((*line_ptr) != (*line) && isspace(*(line_ptr - 1))) || (*line_ptr) == (*line))

/*
 *  Macro for conveniently accessing the current available cell in the external values array for a label node.
 *  It also increments the values_count to indicate the addition of a new value.
 *   
 *  param label_node - Pointer to the symbol_table_node representing the external label
 *  returns - The value of the current available cell in the extern_values array, and increments values_count
 */
#define EXTERN_ARRAY_CURR_AVAILABLE_CELL_PLUS_INC(label_node) label_node -> label_value.label_extern_value.extern_values[(label_node -> label_value.label_extern_value.values_count)++] 

/* works same as EXTERN_ARRAY_CURR_AVAILABLE_CELL_PLUS_INC for param array */
#define PARAM_ARRAY_CURR_AVAILABLE_CELL_PLUS_INC(label_node) label_node -> label_value.label_param_value.param_values[(label_node -> label_value.label_param_value.values_count)++]

/*---------------------------------macro doubly list-----------------------------------------------*/

/*
 *	Creates a new macro node and initializes its values.
 *   
 *	param mcro - The name of the macro to be created
 *	returns - Pointer to the newly created macro node
 */
macro_node *create_macro(char mcro[]) {

    macro_node *new = (macro_node *)malloc(sizeof(macro_node));
    if (new == NULL) {
        errprintf(NO_FILE_ERROR, NO_LINE_ERROR, "Memory allocation failed - macro list");
        exit(1);
    }
    
    strcpy(new -> macro, mcro);
    new -> next = NULL;
    new -> prev = NULL;
    new -> head_line = NULL;
    new -> tail_line = NULL;
    return new;
}


/*
 *	Inserts a new macro node at the end of the list.
 *   
 *	param head - Pointer to the head of the macro list
 *	param tail - Pointer to the tail of the macro list
 *	param mcro - The macro name to be inserted
 */
void insert_macro(macro_node **head, macro_node **tail, char mcro[]) {
	
    macro_node *new;
	new = create_macro(mcro);
    if ((*head) == NULL){ /* macro list is empty */
    	(*head) = new;
    	(*tail) = new;
    }
    
    else{
    
    	(*tail) -> next = new;
    	new -> prev = (*tail);
    	(*tail) = new;
   
    }
    
}

/*
 *	Creates a new line node and initializes its values.
 *   
 *	param macro_n - Pointer to the parent macro node of the line
 *	param line - The content of the line to be stored
 *	returns - Pointer to the newly created line node
 */
line_node *create_line(macro_node **macro_n, char line[]) {
	
	
    line_node *new_line = (line_node *)malloc(sizeof(line_node));
    
    if (new_line == NULL) {
    
		errprintf(NO_FILE_ERROR, NO_LINE_ERROR, "Memory allocation failed - macro list");
        exit(1);
        
    }
    
    strcpy(new_line -> line, line);
    new_line -> next = NULL;
    new_line -> prev = NULL;
    return new_line;
    	
    
}

/*
 *	Inserts a new line node at the end of the list.
 *   
 *	param macro - Pointer to the current macro
 *	param line - The line content to be inserted
 */
void insert_line(macro_node **macro, char line[]) {

    line_node *new = create_line(macro, line);
    if ((*macro) -> head_line == NULL){ /* lines list is empty */
    	(*macro) -> head_line = new;
    	(*macro) -> tail_line = new;
    }
    
    else{
    
    	(*macro) -> tail_line -> next = new;
    	new -> prev = (*macro) -> tail_line;
    	(*macro) -> tail_line = new;
   
    }
    
}

/*
 *	Searches for a macro node with a given macro name.
 *   
 *	param head - Pointer to the head of the macro list
 *	param line - The line of code to check for a macro
 *	returns - Pointer to the found macro node if exists, else NULL
 */
macro_node *is_macro(macro_node **head, char line[]) {
	
	
    macro_node *curr = *head;
    char *line_ptr;
    
    
    while (curr != NULL){
    	if ((line_ptr = strstr(line, curr -> macro)) != NULL)
    		if (MACRO_NAME_DEEP_CHECK)
    			return curr; /* the macro exists in the list */
    		
        curr = curr -> next;
    }
    
    return NULL; /* fail flag */
}

/*
 *	Checks if a macro with the given name exists in the given macro list.
 *   
 *	param head - Pointer to the head of the macro list
 *	param macro_name - The macro name to be checked
 *	returns - 1 if the macro exists, 0 otherwise
 */
int is_macro_exist(macro_node *head, char macro_name[]){
	
	macro_node *curr = head;
	
	while (curr != NULL){
    	if (strcmp(macro_name, curr -> macro) == 0)
    		return 1; /* the macro exists in the list */
    		
        curr = curr -> next;
    }
	
	return 0;
	

}


/*
 *	Writes the lines stored in a macro to a string.
 *   
 *	param des - The destination string to store the macro lines
 *	param macro - Pointer to the macro to be converted
 */
void macro_to_string(char *des, macro_node *macro){

	line_node *curr_line = macro -> head_line;
	
	while (curr_line != NULL){
			
		strcat(des, curr_line -> line);
		curr_line = curr_line -> next;
	
	}

}




/*
 *	Deletes the lines stored in macros.
 *   
 *	param head - Pointer to the head of the macro list
 */
void delete_macro_lines(macro_node **head){

	macro_node *ptr = *(head);
	line_node *l_temp, *l_next;
	
	if (*head == NULL)
		return;
		
	while (ptr != NULL){
		l_temp = ptr -> head_line;
		while (l_temp != NULL){
        	l_next = l_temp -> next;
        	free(l_temp);
        	l_temp = l_next;
    	}
    	ptr -> head_line = NULL;
		ptr -> tail_line = NULL;
		ptr = ptr -> next;
	}
	


}




/*
 *	Frees the memory used by the macro list.
 *   
 *	param head - Pointer to the head of the macro list
 */
void free_macro_list(macro_node **head) {

    macro_node *m_temp = *head;
    macro_node *m_next;
    line_node *l_temp;
    line_node *l_next;
    
   	while (m_temp != NULL) {
        m_next = m_temp -> next;
        l_temp = m_temp -> head_line;
        while (l_temp != NULL){
        	l_next = l_temp -> next;
        	free(l_temp);
        	l_temp = l_next;
       	}
		free(m_temp);
    	m_temp = m_next;
    }
    (*head) = NULL;

}








/*------------------------------------------symbols table BST--------------------------------------*/


/*
 *	Creates a new symbol table node for a label and initializes its values.
 *   
 *	param label - The label name to be stored in the node
 *	param value - The value associated with the label
 *	param type - The type of the label (enum_type)
 *	param comm - The command type of the label (enum_comm)
 *	returns - Pointer to the newly created symbol table node
 */
symbol_table_node *create_label_node(char *label, int value, enum enum_type type, enum enum_comm comm){

    symbol_table_node *new_node = (symbol_table_node *)malloc(sizeof(symbol_table_node));
    
    if (new_node == NULL){
    	errprintf(NO_FILE_ERROR, NO_LINE_ERROR, "Memory allocation failed - symbol table\n");
        exit(1);
    }
    
    /* initialization */
    memset(new_node -> label, '\0', MAX_LABEL_SIZE); /* "clears" junk characters */
    strcpy(new_node -> label, label);
    
    if (type == enum_extl){ /*  if the param type is external */
    	new_node -> label_value.label_extern_value.values_count = 0;
    	EXTERN_ARRAY_CURR_AVAILABLE_CELL_PLUS_INC(new_node) = value; /* array[count++] = value */
    }
    else if (type == enum_type_none){ /*  if the param type is none */
    	new_node -> label_value.label_param_value.values_count = 0;
    	PARAM_ARRAY_CURR_AVAILABLE_CELL_PLUS_INC(new_node) = value;
    }
    else
    	new_node-> label_value.value = value;
    	
    new_node -> type = type;
    new_node -> comm = comm;
    new_node -> left = NULL;
    new_node -> right = NULL;
    return new_node;
}

/*
 *	Inserts a label and its values into the symbol table.
 *   
 *	param root - Pointer to the root of the symbol table
 *	param label - The label to be inserted
 *	param value - The value associated with the label
 *	param type - The type of the label
 *	param comm - The command type of the label
 *	returns - Pointer to the updated root of the symbol table
 */
symbol_table_node *insert_label(symbol_table_node *root, char *label, int value, enum enum_type type,
								enum enum_comm comm){
	
	int cmp;
	
    if (root == NULL) {
        return create_label_node(label, value, type, comm); /* creates new label node */
    }

    cmp = strcmp(label, root -> label);
    
    if (cmp < 0) 
        root -> left = insert_label(root -> left, label, value, type, comm);
        
    else if (cmp > 0) 
        root -> right = insert_label(root -> right, label, value, type, comm);
        
	else{ /* if the label exists in the table */
        
        if (root -> type == enum_type_none){ /* if the exists label is not defined yet */
        
        	root -> type = type; /* changes the type */
        	
        	if (comm != enum_comm_none)
        		root -> comm = comm;
        
        	if (type == enum_rel || type == enum_ent)
        		root -> label_value.value = value;

        	else if (type == enum_type_none){
        		/* inserts the value to the parameter label array and increases the array counter */
				PARAM_ARRAY_CURR_AVAILABLE_CELL_PLUS_INC(root) = value;
        		
        	}
        	
        	/* else - if the param type = external the external array
        	   will be "updated automatically" (because of the union) */
        
        
        }
        /* if the label is already an entry - updates the command from none to the param comm */
        else if (root -> type == enum_ent){
        	if ((type == enum_rel)){ /* assumes root -> comm = enum_comm_none */
           		root -> label_value.value = value;
        		root -> comm = comm;
        	}
        	/* root -> comm not equal to enum_dir/enum_ins and param type not equal to enum_extl
        	   (if so the error will be found in the first run),
        	   if param type = enum_ent/enum_type_none - nothing changes  */
        	
        }
        
        /* if the label is already an external - updates new value */
        else if (root -> type == enum_extl){
        	if (type == enum_type_none){ /* if the param type is enum_type_none */
        		/* updates from no value to the param value if the label is used in an instruction */
        		if (root -> label_value.label_extern_value.extern_values[(root -> label_value.label_extern_value.values_count) - 1] == NO_VALUE)
        			root -> label_value.label_extern_value.extern_values[(root -> label_value.label_extern_value.values_count) - 1] = value;
        		else
        			/* inserts the value to the external label array and increases the array counter */
        			EXTERN_ARRAY_CURR_AVAILABLE_CELL_PLUS_INC(root) = value;
        	}
        	
        	/* param type cannot be enum_rel/enum_ent(it is a first run error)
        	   if param type is enum_extl nothing changes (redeclaration of extern is not an error) */
        }
        
		else { /* the label is relocatable type */
			
			if (type == enum_ent)
				root -> type = enum_ent;
			
			/* if the param type = enum_rel/enum_type_none, nothing changes. 
			   the param type cannot be enum_extl(if so the error will be found in the first run) */
			
		}

        
    }

    return root;
}

/*
 *	Searches for a label in the symbol table.
 *   
 *	param root - Pointer to the root of the symbol table
 *	param label - The label to be searched
 *	returns - Pointer to the found label node if exists, else NULL
 */
symbol_table_node *search_label(symbol_table_node *root, char *label){

	int cmp;
	
    while (root != NULL) {
    
        cmp = strcmp(label, root -> label);
        
        if (cmp == 0) /* label found */
        	return root;

            
        else if (cmp < 0)
            root = root -> left;
            
         else 
            root = root -> right;
        
    }

    return NULL; /* label not found */
}

/*
 *	Updates the type of a label in the symbol table.
 *   
 *	param label_node - Pointer to the label node to be updated
 *	param type - The new type of the label
 */
void update_label_type(symbol_table_node *label_node, enum enum_type type){

	label_node -> type = type;

}


/*
 *	Increases the value of labels of a certain type in the symbol table.
 *	Used to update the directive labels according the IC.
 *   
 *	param root - Pointer to the root of the symbol table
 *	param add - The value to be added to the labels
 *	param comm - The command type of the labels to be updated
 */
void increase_labels_value_by_comm(symbol_table_node *root, int add, enum enum_comm comm){
	
	int i;
	
	if (root == NULL)
		return;
		
	if (root -> comm == comm){
		if (root -> type == enum_extl){
			for (i = 0; root -> label_value.label_extern_value.values_count; i++){
			
				root -> label_value.label_extern_value.extern_values[i] += add;
		
			}
	
		}
		else
			root -> label_value.value += add;
	}

		
	increase_labels_value_by_comm(root -> left, add, comm);
	increase_labels_value_by_comm(root -> right, add, comm);
}


/*
 *	Prints external labels to a file stream in post order (to get lexicographic order).
 *   
 *	param des - Pointer to the file stream to write to
 *	param root - Pointer to the root of the symbol table
 */
void print_extern_labels_to_stream(FILE *des, symbol_table_node *root){

	int i;
	
	if (root == NULL)
		return;
		
	print_extern_labels_to_stream(des, root -> left);
	print_extern_labels_to_stream(des, root -> right);
	
	/* if it is used external label */
	if (root -> type == enum_extl &&
	    root -> label_value.label_extern_value.extern_values[0] != NO_VALUE){
		for (i = 0; i < root -> label_value.label_extern_value.values_count; i++){
			
			fprintf(des, "%s\t%d\n", root -> label, root -> label_value.label_extern_value.extern_values[i] + MEMORY_ASSUMPTION);
		}
	}

}


/*
 *	Prints entry labels to a file stream in post order (to get lexicographic order).
 *   
 *	param des - Pointer to the file stream to write to
 *	param root - Pointer to the root of the symbol table
 */
void print_entry_labels_to_stream(FILE *des, symbol_table_node *root){
	
	if (root == NULL)
		return;
	
	print_entry_labels_to_stream(des, root -> left);
	print_entry_labels_to_stream(des, root -> right);
	
	if (root -> type == enum_ent)
		fprintf(des, "%s\t%d\n", root -> label, root -> label_value.value + MEMORY_ASSUMPTION);
		
}

/*
 *	Checks if there are any entry labels in the symbol table.
 *   
 *	param root - Pointer to the root of the symbol table
 *	returns - 1 if entry label exists, 0 otherwise
 */
int is_there_entry(symbol_table_node *root){
	
	if (root == NULL)
		return 0;

	if (root -> type == enum_ent)
		return 1;
	
	return is_there_entry(root -> left) || is_there_entry(root -> right);
	
}


/*
 *	Checks if there are used external labels in the symbol table or its sub-trees.
 *   
 *	param root - Pointer to the root of the symbol table or its sub-tree
 *	returns - 1 if used external labels are found, 0 otherwise
 */
int is_there_used_extern(symbol_table_node *root){
	
	if (root == NULL)
		return 0;
		
	/* if it is external label and is used in the code by instruction */
	if (root -> type == enum_extl && root -> label_value.label_extern_value.extern_values[0] != NO_VALUE)
		return 1;
	
	return is_there_used_extern(root -> left) || is_there_used_extern(root -> right);
	
}



/*
 *	Exports declared entry and (used) external labels to .ent and .ext files.
 *   
 *	param file_name - The base name of the output files
 *	param root - Pointer to the root of the symbol table
 */
void export_entry_and_extern_labels(char *file_name, symbol_table_node *root){
		
	FILE *ent_des, *ext_des;
	char temp_name[MAX_BUFFER];	
	
	if (is_there_entry(root)){ /* if there are entry labels in the table */
	
		sprintf(temp_name, "%s.ent", file_name);
		if (!(ent_des = fopen(temp_name, "w"))){
	
			errprintf(temp_name, NO_LINE_ERROR, "cannot write file");
			return;
		
		}
		print_entry_labels_to_stream(ent_des, root); /* writes in .ent file the entry lables */
		fclose(ent_des);
	}
	
	if (is_there_used_extern(root)){ /* if there are used external labels in the table */
	
		sprintf(temp_name, "%s.ext", file_name);
		if (!(ext_des = fopen(temp_name, "w"))){
	
			errprintf(temp_name, NO_LINE_ERROR, "cannot write file");
			return;
		
		}
		print_extern_labels_to_stream(ext_des, root); /* writes in .ext file the external lables */
		fclose(ext_des);
	}

}



/*
 *	Frees the memory used by the symbol table.
 *   
 *	param root - Pointer to the root of the symbol table
 */
void free_symbol_table(symbol_table_node **root){

    if (*root != NULL) {
    
        free_symbol_table(&((*root) -> left));
        free_symbol_table(&((*root) -> right));
        free(*root);
        
    }
    *root = NULL;
}



