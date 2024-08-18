/*
 * This file contains functions for working with a stack.
 * The stack is implemented as an array and is used to push and pop numbers.
 * The functions in this file provide operations such as pushing a number onto the stack,
 * popping the top element from the stack, and checking if the stack is empty.
 * In this program the stack is used as efficient way to insert the elements in the circular list in descending order.
 *
 * Author: Gal Levi
 * Version: 31.05.22
*/
 
 
#include "list.h"
#include "fib_stack.h"

#define EMPTY -1 /* represents an empty stack index */

static int top = EMPTY;
static unsigned long stack[MAX_N];


/*
 * Pushes a number onto the stack.
 *
 * param num - The number to be pushed onto the stack.
 * returns: 1 if the push operation is successful, 0 otherwise.
 */
int push(unsigned long num){

	if (top == MAX_N){
		printf("stack overflow\n");
		return 0;
	}
	
	stack[++top] = num;

	return 1;
} /* end of function push */

/*
 * Pops the top element from the stack.
 *
 * returns: The popped element.
 */
unsigned long pop(){

	if (top == EMPTY){
		printf("stack underflow\n");
		return 0;
	}
	
	return stack[top--];

} /* end of function pop */

/*
 * Checks if the stack is empty.
 *
 * returns: 1 if the stack is empty, 0 otherwise.
 */
int isEmpty(){

	return top == EMPTY;

} /* end of function isEmpty */


