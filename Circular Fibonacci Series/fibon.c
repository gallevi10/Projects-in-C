/*
 * This program generates the Fibonacci series up to a given position and stores it in a circular linked list.
 * The program prompts the user to enter a number between 0 and 46 (in accordance to the limit of bits of unsigned long),
 * representing the position of the Fibonacci series.
 * It then generates the series and stores it in a circular linked list.
 * The program prints the generated series to the screen and saves it to a file specified by the command-line argument.
 * The series is printed in descending order in both the screen and the file.
 *
 * Author: Gal Levi
 * Version: 31.05.22
*/

#include "fibon.h"

/*
 * Main function of the program.
 *
 * Reads a number from the user, generates the Fibonacci series up to that number, and stores it in a circular list.
 * The generated series is then printed to the screen and saved in the user file in descending order.
 *
 * param argc - The number of command-line arguments.
 * param argv - The array of command-line arguments.
 * returns: 0 on successful execution.
 */
int main(int argc, char *argv[]) {

	FILE *fd; /* user file pointer */
	int n, check_scan; /* n - stores the user input, check_scan - stores the scanf return value of the input */
	node* head = NULL; /* initializes the circular list */
	
	if (argc > ARGS){ /* if there are more than 2 arguments */
		printf("Too much arguments\n");
		return 0;
	}
		
	if (argc < ARGS){ /* if the name of file is not entered */
		printf("Missing file name\n");
		return 0;
	}
	
	if (!(fd = fopen(FILE_NAME, "w+"))){ /* FILE_NAME - the name of user file */
		
		printf("Cannot open file\n");
		return 0;
	
	}
	
	printf("Please enter a number between 0 to 46 (position of Fibonacci series):\n");
	
	while (FOREVER){ /* loops until user enters a valid n */
		if ((check_scan = scanf("%d", &n)) != 1){
			if (check_scan == EOF){
				fclose(fd); /* if EOF/ctrl+D is entered */
				return 0;
			}
			printf("Invalid input - not a number, try again...\n");
			getchar(); /* cleans the buffer */
		}
	
		else if (n > MAX_N) /* MAX_N = 46 */
			printf("Invalid input - too big number, try again...\n");
	
		else if (n < 0)
			printf("Invalid input - negative number, try again...\n");
		
		else
			break;

	} /* end of while */
	
	
	fib_to_stack(n); /* pushes the Fibonacci numbers to the stack */
	
	stack_to_list(&head); /* inserts the Fibonacci numbers to list in descending order */
	
	print_list(stdout, head); /* prints the list to screen */
	
	fprintf(fd, "Fibonacci series numbers in descending order from position n = %d to 0:\n", n);
	print_list(fd, head); /* prints the list to the user file */
	
	fclose(fd);
	
	
	free_list(&head); /* frees the whole list */
	



    return 0;
} /* end of function main */




