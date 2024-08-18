/*
 * This file contains functions that executes operations on complex numbers.
 * Author: Gal Levi
 * Version: 19.05.22
*/


#include "complex.h"



/*
 * Reads the real and imaginary parts of a complex number.
 *
 * param A: Pointer to the complex number structure.
 * param real: Real part of the complex number.
 * param img: Imaginary part of the complex number.
 */
void read_comp(complex * A, double real, double img){

	
	A -> r = real; /* assigns the real part of the complex number */
	A -> i = img; /* assigns the imaginary part of the complex number */


} /* end of function read_comp */

/*
 * Prints a complex number in the format a+bi or a-bi with precision of 2 decimals after dot.
 *
 * param A: Pointer to the complex number structure.
 */
void print_comp(complex * A){


	if ((A -> i) == 0)
		printf("%.2f", A -> r); /* if the imaginary part is 0, prints only the real part */
	else if ((A -> r) == 0)
		printf("%.2fi", A -> i); /* if the real part is 0, prints only the imaginary part */
	else if ((A -> i) == 0 && (A -> r) == 0)
		printf("%.2f", 0.00); /* if the real and the imaginary parts are 0, prints 0 */
	else if ((A -> i) < 0)
		printf("%.2f-(%.2f)i", A -> r, -(A -> i)); /* if the imaginary part is negative, prints as a-bi */
	else
		printf("%.2f+(%.2f)i", A -> r, (A -> i)); /* Otherwise, prints as a+bi */


} /* end of function print_comp */

/*
 * Adds two complex numbers and returns the result.
 *
 * param A: Pointer to the first complex number.
 * param B: Pointer to the second complex number.
 *
 * returns: Result of the addition as a complex number.
 */
complex add_comp(complex * A, complex * B){

	complex result; /* initializes the function output */
	
	result.r = (A -> r) + (B -> r); /* adds the real parts of A and B */
	result.i = (A -> i) + (B -> i); /* adds the imaginary parts of A and B */
	
	return result;

} /* end of function add_comp */


/*
 * Subtracts two complex numbers and returns the result.
 *
 * param A: Pointer to the first complex number.
 * param B: Pointer to the second complex number.
 *
 * returns: Result of the subtraction as a complex number.
 */
complex sub_comp(complex* A, complex* B){

	
	complex result;
	
	result.r = (A -> r) - (B -> r); /* subtracts the real parts of A and B */
	result.i = (A -> i) - (B -> i); /* subtracts the imaginary parts of A and B */
	
	return result;


} /* end of function sub_comp */


/*
 * Multiplies a complex number by a real number and returns the result.
 *
 * param A: Pointer to the complex number.
 * param real: Real number to multiply with.
 *
 * returns: Result of the multiplication as a complex number.
 */
complex mult_comp_real(complex* A, double real){

	
	complex result;
	
	result.r = (A -> r) * real; /* multiplies the real part of A by the real number parameter */
	result.i = (A -> i) * real; /* multiplies the imaginary part of A by the real number parameter */
	
	return result;

} /* end of function mult_comp_real */

/*
 * Multiplies a complex number by an imaginary number and returns the result.
 *
 * param A: Pointer to the complex number.
 * param img: Imaginary number to multiply with.
 *
 * returns: Result of the multiplication as a complex number.
 */
complex mult_comp_img(complex* A, double img){

	
	complex result;
	
	result.r = -((A -> i) * img); /* multiplies the imaginary part of A by the imaginary number parameter */
	result.i = (A -> r) * img; /* multiplies the real part of A by the imaginary number parameter */
	
	return result;


} /* end of function mult_comp_img */

/*
 * Multiplies two complex numbers and returns the result.
 *
 * param A: Pointer to the first complex number.
 * param B: Pointer to the second complex number.
 *
 * returns: Result of the multiplication as a complex number.
 */
complex mult_comp_comp(complex* A, complex* B){

	
	complex result;
	
	/* computes the real part of the result complex number and assigns it*/
	result.r = ((A -> r) * (B -> r)) - ((A -> i) * (B -> i)); 
	/* computes the imaginary part of the result complex number and assigns it */
	result.i = ((A -> r) * (B -> i)) + ((A -> i) * (B -> r)); 
	
	return result;


} /* end of function mult_comp_comp */

/*
 * Computes the absolute value of a complex number.
 *
 * param A: Pointer to the complex number.
 *
 * returns: Absolute value of the complex number.
 */
double abs_comp(complex* A){

	
	double result;
	
	/* computes the absolute number of the complex variable parameter value and assigns it to result */
	result = sqrt((A -> r) * (A -> r) + (A -> i) * (A -> i));
	
	return result;


} /* end of function abs_comp */




