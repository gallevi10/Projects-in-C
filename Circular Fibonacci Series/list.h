/*
 * This file contains struct and functions prototype of file circular_list.c
 * Author: Gal Levi
 * Version: 31.05.22
*/


#include <stdio.h>
#include <stdlib.h>

/* circular list struct */
typedef struct node {
    unsigned long num;
    struct node *next;
} node;


/* functions prototype */
node *create_node(unsigned long);
void insert_node(node **, unsigned long);
void print_list(FILE *, node *);
void free_list(node **);


