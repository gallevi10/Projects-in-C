/*
 * This file contains macros and functions prototype of file fibon.c
 * Author: Gal Levi
 * Version: 31.05.22
*/


#include "list.h"
#include "fib_stack.h"

#define ARGS 2 /* defines the number of expected arguments */
#define FILE_NAME argv[1] /* user file name */
#define FOREVER 1 /* for infinite loop */


/* functions prototype */
void fib_to_stack(int);
void stack_to_list(node **);
