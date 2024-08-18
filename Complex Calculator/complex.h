/*
 * This file contains struct declarations, enum, and functions prototype.
 * Author: Gal Levi
 * Version: 19.05.22
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

/* numbering the possible commands in order */
enum command_code {r_c = 1, p_c, ad_c, s_c, m_c_r, m_c_i, m_c_c, ab_c, s};

/* new struct which represents a complex number */
typedef struct {
	
	double r; /* the real number */
	
	double i; /* the imaginary number */
	
} complex;

/* new struct which represents a command and its possible parameters */
typedef struct {
	
	int com; /* the command number */
	
	char comp_var1; /* first complex variable */
	
	char comp_var2; /* second complex variable */
	
	double num1; /* real/imaginary number - depends on command */
	
	double num2; /* imaginary number */
	
} command;



/* function prototype */
void read_comp(complex *, double, double);
void print_comp(complex *);
complex add_comp(complex *, complex *);
complex sub_comp(complex *, complex *);
complex mult_comp_real(complex *, double);
complex mult_comp_img(complex *, double);
complex mult_comp_comp(complex *, complex *);
double abs_comp(complex *);
command get_command();



