/*
 * This file contains functions for working with a circular linked list.
 * The circular linked list is used to store and manipulate numbers.
 * The functions in this file provide operations such as creating a new node,
 * inserting a node at the end of the list, printing the list, and freeing the list.
 *
 * Author: Gal Levi
 * Version: 31.05.22
*/


#include "list.h"



/*
 * Creates a new node with the given number.
 *
 * param number - The number to be stored in the node.
 * returns: Pointer to the newly created node.
 */
node *create_node(unsigned long number) {

    node *new_node = (node *)malloc(sizeof(node));
    if (!new_node){ /* if allocation failed */
    	printf("There is not enough memory - cannot build list, please try again...\n");
    	exit(0);
    }
    
    new_node -> num = number;
    new_node -> next = NULL;
    return new_node;
    
} /* end of function create_node */

/*
 * Inserts a new node with the given number at the end of the circular linked list.
 *
 * param head - Pointer to the head of the circular linked list.
 * param number - The number to be inserted.
 */
void insert_node(node **head, unsigned long number) {

	/* last - pointer to the last element in list */
    node* last, *new_node = create_node(number);
    
    if (*head == NULL) { /* if the list is empty */
    	
    	/* makes the param node the first node of the list */
        *head = new_node;
        new_node -> next = *head;
    } /* end of if */
    
    else {
    
        last = *head;
        while (last -> next != *head)
            last = last -> next;
            
        /* inserts the new element right after the last element */
        new_node -> next = *head;
        last -> next = new_node;
    } /* end of else */
} /* end of function insert node */


/*
 * Prints the elements of the circular linked list to the given file pointer.
 *
 * param fd - File pointer to write the list to.
 * param head - Pointer to the head of the circular linked list.
 */
void print_list(FILE *fd, node *head) {

    node *current = head; /* list pointer */
    
    if (head == NULL) {
        fprintf(fd, "List is empty.\n");
        return;
    }
    

    do {
        fprintf(fd, "%lu ", current -> num);
        current = current -> next;
    } while (current != head);
    
    fprintf(fd, "\n");
} /* end of function print_list */

/*
 * Frees the memory occupied by the circular linked list.
 *
 * param head - Pointer to the head of the circular linked list.
 */
void free_list(node **head){

	node *current_node = *head, *next_node = current_node -> next;

	
	if (current_node == NULL) /* the list is empty */
		return;
	
	
	while (next_node != (*head)) {
		free(current_node);
		current_node = next_node;
	    next_node = current_node -> next;
	} /* end of while */
	
	free(current_node);

} /* end of function free_list */




