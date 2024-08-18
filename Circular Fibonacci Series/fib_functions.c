/*
 * This file contains fibon.c functions for generating the Fibonacci series and transferring the series
 * from a stack to a circular linked list.
 * The `fib_to_stack` function generates the Fibonacci series up to a given number and pushes the
 * series to a stack.
 * The `stack_to_list` function transfers the elements from the stack to a
 * circular linked list.
 *
 * Author: Gal Levi
 * Version: 31.05.22
 */

#include "fibon.h"



/*
 * Generates the Fibonacci series up to the given number and pushes the series to a stack.
 *
 * param n - The position of the Fibonacci series to generate.
 */
void fib_to_stack(int n){
	
	int i;
	unsigned long a = 1, b = 1;
	
	if (n == 0)
		push(a);
		
	else{
	
		push(a);
		push(b);
		
		/* loop to push Fibonacci numbers to the stack */
		for (i = 0; i <= n-2; i++){ /* n-2 because we already pushed 2 numbers */
			
			b = a + b;
			push(b);
			a = b - a;
		
		} /* end of for */
	
	} /* end of else */
	

} /* end of function fib_to_stack */



/*
 * Transfers the elements from the stack to a circular linked list.
 *
 * param head_ptr - Pointer to the head of the circular linked list.
 */
void stack_to_list(node **head_ptr){
	
	
	while (!isEmpty()){ /* while stack is not empty */
	
		insert_node(head_ptr, pop()); /* inserts elements from stack to the list */
	
	}
	

} /* end of function stack_to_list */

