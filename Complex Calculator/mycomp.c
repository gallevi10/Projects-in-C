/*
 * A program that receives commands about complex numbers from the user and executes them.
 * Author: Gal Levi
 * Version: 19.05.22
*/


#include "complex.h"
#define FOREVER 1 /* for infinite loop */

/* functions prototype */
complex *find_comp(char, complex *[]);
void menu();

/* the main function of the program */
int main (){
	
	/* initializing the complex variables */
	complex A = {0, 0};
	complex B = {0, 0};
	complex C = {0, 0};
	complex D = {0, 0};
	complex E = {0, 0};
	complex F = {0, 0};
	
	/* functions result */
	complex func_res;
	double abs_res;
	
	command user_command; /* the command received from the user */


	/* array of addresses of the complex variables */
	complex *comp_vars[6];
	
	/* assigns the addresses in order */
	comp_vars[0] = &A;
	comp_vars[1] = &B;
	comp_vars[2] = &C;
	comp_vars[3] = &D;
	comp_vars[4] = &E;
	comp_vars[5] = &F;
	
	
	
	
	menu(); /* prints the program menu */
	
	while (FOREVER){
	
		puts("\nWaiting for input...\n");
		user_command = get_command(); /* getting the command input after analyzing */
	
		switch(user_command.com){
			
			/* r_c = 1 - read_comp command code
			updates the chosen complex variable according to user input */
			case r_c: read_comp(find_comp(user_command.comp_var1, comp_vars), 
							user_command.num1, user_command.num2);
			break;
			
			/* p_c = 2 - print_comp command code
			prints the chosen complex variable */
			case p_c: print_comp(find_comp(user_command.comp_var1, comp_vars));
					printf("\n");
			break;
			
			/* ad_c = 3 - add_comp command code
			prints the addition of the chosen complex variables */
			case ad_c: func_res = add_comp(find_comp(user_command.comp_var1, comp_vars), 
										find_comp(user_command.comp_var2, comp_vars));
										
					print_comp(find_comp(user_command.comp_var1, comp_vars)); /* first complex variable */
					printf(" + ");
					print_comp(find_comp(user_command.comp_var2, comp_vars)); /* second complex variable */
					printf(" = ");
					print_comp(&func_res); /* the addition result */
	
					printf("\n");
			break;
			
			/* s_c = 4 - sub_comp command code
			prints the subtraction of the chosen complex variables */
			case s_c: func_res = sub_comp(find_comp(user_command.comp_var1, comp_vars), 
										find_comp(user_command.comp_var2, comp_vars));
										
						print_comp(find_comp(user_command.comp_var1, comp_vars));
						printf(" - ");
						print_comp(find_comp(user_command.comp_var2, comp_vars));
						printf(" = ");
						print_comp(&func_res);
	
						printf("\n");
			break;
			
			/* m_c_r = 5 - mult_comp_real command code
			prints the multiplication of the chosen complex variable and real number */
			case m_c_r: func_res = mult_comp_real(find_comp(user_command.comp_var1, comp_vars), 
									user_command.num1);
					print_comp(find_comp(user_command.comp_var1, comp_vars)); /* the complex number */
					printf(" * %.2f = ", user_command.num1); /* the real number */
					print_comp(&func_res);
	
					printf("\n");

			break;
			
			/* m_c_i = 6 - mult_comp_img command code
			prints the multiplication of the chosen complex variable and imaginary number */		
			case m_c_i: func_res = mult_comp_img(find_comp(user_command.comp_var1, comp_vars), 
									user_command.num1);
					print_comp(find_comp(user_command.comp_var1, comp_vars));
					printf(" * ");
					printf("(%.2f)i", user_command.num1); /* the imaginary number */
					printf(" = ");
					print_comp(&func_res);
	
					printf("\n");
			break;
			
			/* m_c_c = 7 - mult_comp_comp command code
			prints the multiplication of the chosen complex variables */
			case m_c_c: func_res = mult_comp_comp(find_comp(user_command.comp_var1, comp_vars), 
									find_comp(user_command.comp_var2, comp_vars));
					
					print_comp(find_comp(user_command.comp_var1, comp_vars)); /* the first complex variable */
					printf(" * ");
					print_comp(find_comp(user_command.comp_var2, comp_vars)); /* the second complex variable */
					printf(" = ");
					print_comp(&func_res);
	
					printf("\n");
			break;
			
			/* ab_c = 8 - abs_comp command code 
			prints the absolute value of the chosen complex variable */
			case ab_c: abs_res = abs_comp(find_comp(user_command.comp_var1, comp_vars));
					
					printf("|");
					print_comp(find_comp(user_command.comp_var1, comp_vars));
					printf("| = %.2f\n", abs_res);
				
			break;
			
			/* s = 9 - stop command code
			stops the program */					
			case s: printf("You chose to stop the program.\n");
					return 0;
		} /* end of switch */
		
	} /* end of while */
	
	return 0;
} /* end of function main */


/**
 * Finds the complex variable based on the given character identifier.
 *
 * param comp_var: The character identifier of the complex variable to find.
 * param comp_vars: An array of complex pointers.
 * returns: A pointer to the complex variable found.
 */
complex *find_comp(char comp_var, complex *comp_vars[]){
	
	
	switch(comp_var){
	
		case 'A': return comp_vars[0]; /* comp_vars[0] = address of complex variable A */

		case 'B': return comp_vars[1];

		case 'C': return comp_vars[2];

		case 'D': return comp_vars[3];

		case 'E': return comp_vars[4];
		
		/* case 'F' */
		default: return comp_vars[5]; 

	}
	
	
} /* end of function find_comp */

/**
 * Displays the program menu.
 */
void menu(){

	printf("***Menu***\n\n");
	printf("Enter read_comp X, Y, Z - to update the complex variable X with the real number Y and imaginary number Z.\n");
	printf("Enter print_comp X - to print the current value of the complex variable X.\n");
	printf("Enter add_comp X, Y - to print the addition result of complex variables X and Y.\n");
	printf("Enter sub_comp X, Y - to print the subtraction result of complex variables X and Y.\n");
	printf("Enter mult_comp_real X, Y - to print the multiplication result of complex variable X and real number Y.\n");
	printf("Enter mult_comp_img X, Y - to print the multiplication result of complex variable X and imaginary number Y.\n");
	printf("Enter mult_comp_comp X, Y - to print the multiplication result of complex variable X and complex variable Y.\n");
	printf("Enter abs_comp X - to print the absolute number of complex variable X.\n");
	printf("Enter stop - to stop the program.\n\n");


} /* end of function menu */
	

