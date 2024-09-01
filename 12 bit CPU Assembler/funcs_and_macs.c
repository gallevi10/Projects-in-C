/*
 *	File: funcs_and_macs.c
 *
 *  This file contains various utility functions used throughout the assembler program.
 *	These include string manipulation, whitespace handling, error and warning printing functions,
 *	and ANSI escape codes for text formatting and color.
 *	The functions defined here are used to perform common tasks and improve the readability of the code.
 *
 *  author: Gal Levi
 *  version: 5.8.23
 */


#include "funcs_and_macs.h"
#include <stdarg.h>

/* macro definitions */
#define RES_WORDS_NUM 28 /* number of reserved words in the assembly language */
#define ANSI_COLOR_RED     "\x1b[31m" /* ANSI escape code for red text color */
#define ANSI_COLOR_PURPLE  "\x1b[35m" /* ANSI escape code for purple text color */
#define ANSI_BOLD          "\x1b[1m" /* ANSI escape code for bold text style */
#define ANSI_STYLE_RESET   "\x1b[0m" /* ANSI escape code to reset text style and color */





/*
 * This function removes spaces and tabs from a given string.
 * It modifies the original string by shifting the remaining characters to remove the spaces and tabs.
 * The resulting string does not contain any spaces or tabs.
 * 
 * param str - A pointer to the string to be modified.
 */
void remove_white(char *str) {

    char *temp = str;
    
    do {
        while (isspace(*temp)) {
        	++temp;
        } /* end of inner while */
        
    } while ((*str++ = *temp++));
}



/*
 *  Checks if a string consists only of white space characters.
 *
 *  param str - A pointer to the string to be checked.
 *  returns 1 if the string contains only white space characters, 0 otherwise.
 */
int is_white(char *str){

	char *ptr = str;
	
	while (*ptr != '\0'){
		
		if (!isspace(*ptr))
			return 0;
		ptr++;
	}
	return 1;

}

/*
 *  Checks if the first n characters of a string are white space characters.
 *
 *  param str - A pointer to the string to be checked.
 *  param n - The number of characters to check.
 *  returns 1 if the first n characters are white space, 0 otherwise.
 */
int is_white_n(char *str, int n){

	int i;
	
	for (i = 0; i < n; i++){
		
		if (!isspace(str[i]))
			return 0;
	}
	return 1;

}

/*
 *  Counts the occurrences of a specified character in a string.
 *
 *  param str - A pointer to the string to be checked.
 *  param ch - The character to be counted.
 *  returns The number of occurrences of the character in the string.
 */
int count_char(char *str, char ch){

	int i, cnt = 0;
	
	for (i = 0; str[i]; i++)
		if (str[i] == ch)
			cnt++;
	
	return cnt;

}

/*
 *  Checks if a string is separated by white space.
 *
 *  This function checks if a string is separated by white space characters and returns 1 if it is.
 *  It analyzes the arrangement of white space characters within the string.
 *
 *  param str - A pointer to the string to be checked.
 *  returns 1 if the string is separated by white space, 0 otherwise.
 */
int is_sep_by_white(char *str){
	
	int white_flag = 0, white_part_cnt = 0;
	char *ptr;
	
	ptr = str;
	
	while (*ptr){ /* != '\0' */
		
		if (isspace(*ptr)){
				
			if (white_flag == 0) {
				white_flag = 1;
				white_part_cnt++;
			}
		}
			
		else if (white_flag == 1) /* if the character is non-white character and white_flag is on */
			white_flag = 0;
		
		if (white_part_cnt > 0 && white_flag == 0)
			return 1; /* the string seperated by white parts */
		
		ptr++;
	
	}

	return 0; /* the string is not seperated by white parts */


}

/*
 *  Checks if a string represents a numeric value according the assignment demand.
 *
 *  param str - A pointer to the string to be checked.
 *  returns 1 if the string represents a numeric value, 0 otherwise.
 */
int is_num(char *str){
	
	int i;
	
	for (i = 0; str[i]; i++){
		
		/* if the number starts with '+' or '-' - it is valid */
		if (i == 0 && (str[i] == '-' || str[i] == '+'))
			continue;
				
		if (!isdigit(str[i]))
			return 0;
				
		
		
	}
	return 1;	

}

/*
 *  Checks if a string is a reserved word.
 *
 *  This function checks if the given string matches any of the reserved words.
 *
 *  param str - A pointer to the string to be checked.
 *  returns 1 if the string is a reserved word or directive, 0 otherwise.
 */
int is_reserved_word(char *str){
	
	int i;
	const char *RES_WORDS[] = {"@r0", "@r1", "@r2", "@r3", "@r4", "@r5", "@r6", "@r7",
								"mov", "cmp", "add", "sub", "not", "clr", "lea", "inc",
								 "dec", "jmp", "bne", "red", "prn", "jsr", "rts", "stop",".string", 
								 ".data", ".entry", ".extern"};

	for (i = 0; i < RES_WORDS_NUM; i++){ /* RES_WORDS_NUM = 28 */
		if (strcmp(RES_WORDS[i], str) == 0){ /* if str is a reserved word */
			return 1;
		}
	}
	
	return 0;

}

/*
 *  Checks if a string consists of numeric characters and punctuation.
 *
 *  param str - A pointer to the string to be checked.
 *  returns 1 if the string consists of numeric characters and punctuation, 0 otherwise.
 */
int is_num_and_punc(char *str){
	
	int i;
	
	for (i = 0; str[i]; i++){
				
		if (!isdigit(str[i]) && !ispunct(str[i]))
			return 0;

	}
	return 1;	

}

/*
 *  Removes leading and trailing white space from a string.
 *
 *  This function removes white space characters from the beginning and end of the string.
 *
 *  param str - A pointer to the string to be modified.
 */
void remove_white_from_borders(char *str){

	int start, end;
	char *ptr;
	
	if (is_white(str)){	 /* edge case - if it is a white chatacters line */
		if ((ptr = strchr(str, '\n'))) /* removes new line character of white line */
			*ptr = '\0';
		if ((ptr = strchr(str, '\r'))) /* removes return character of white line */
			*ptr = '\0';
		return;
	}

		
	/* removes the white characters from the borders of str */
	start = 0;
	JUMP_TO_NEXT_NON_WHITE(str, start)
	end = strlen(str) - 1;
	JUMP_BACKWARDS_TO_FIRST_NON_WHITE(str, end)
	/* start is the index of the first non-white character of str from the start of the string
		end is the index of the first non-white character of str from the end of the string */
	
	memmove(str, str + start, end - start + 1);
	
    str[end - start + 1] = '\0';


}


/*
 *  Validates a label or macro name.
 *
 *  This function validates a label or macro name according to assembler rules. It checks for
 *  various conditions such as the length, alphanumeric characters, reserved words, and more.
 *
 *  param lm - The label or macro name to be validated.
 *  param is_label - 1 if the provided name is a label, 0 if it's a macro.
 *  returns An integer code indicating the validation result (see details in the code comments).
 */
int is_valid_lm(char *lm, int is_label){

	int i;
	
	remove_white_from_borders(lm); /* removes whites from the borders of the string */
	
	
	if (is_white(lm)) /* the label/macro is an empty word */
		return 2;
	
	if (!isalpha(lm[0])) /* the label/macro starts with unalphabetic character */
		return 3;
	
	
	if (is_sep_by_white(lm)) /* the label/macro is seperated by white characters */
		return 4;
	
	/* the label is too long - more than 31 characters */
	if (is_label && strlen(lm) > (MAX_LABEL_SIZE-1)) /* MAX_LABEL_SIZE-1 = 31 */
		return 5;	

	if (is_reserved_word(lm)) /* the label/macro is a reserved word */
		return 6;
	
	for (i = 0; i < strlen(lm); i++){
		if (!isalnum(lm[i])) /* the label/macro contains unalphanumeric character */
			return 7;
	}
		
	return 1;	  /* the label/macro is valid */ 
								  

}

/*
 *  Checks if a line is a comment.
 *
 *  This function checks if a given line is a comment by looking for the ';' character.
 *
 *  param curr_line - A pointer to the line to be checked.
 *  returns:
 *	1 if the line is a comment starting with whitespace,
 *	2 if it's a comment starting without whitespace, 0 otherwise.
 */
int is_comment(char *curr_line){
	char *ptr;
	
	if ((ptr = strchr(curr_line, ';'))){
		if (is_white_n(curr_line, ptr - curr_line))
			return 1;
		else
			return 2;
	}
	return 0;
		
}




/*
 *  Prints formatted error messages to stderr.
 *
 *  This function prints formatted error messages to the standard error stream (stderr).
 *
 *  param file_name - The name of the source file related to the error.
 *  param line - The line number related to the error, or NO_LINE_ERROR if not applicable.
 *  param error - The error message format string.
 *  ... - Additional arguments to format the error message.
 */
void errprintf(const char file_name[], const int line, const char *error, ...) {
    va_list args;
    va_start(args, error);

    if (strcmp(NO_FILE_ERROR, file_name))
    	fprintf(stderr, "File " ANSI_BOLD "'%s': " ANSI_STYLE_RESET, file_name);
    if (line != NO_LINE_ERROR)
    	fprintf(stderr, "Line %d: ", line);
    fprintf(stderr, ANSI_BOLD ANSI_COLOR_RED "Error: " ANSI_STYLE_RESET);

    vfprintf(stderr, error, args);
    
    fprintf(stderr, "\n");

    va_end(args);
}


/*
 *  Prints formatted warning messages to stdout.
 *
 *  This function prints formatted warning messages to the standard output stream (stdout).
 *
 *  param file_name - The name of the source file related to the warning.
 *  param line - The line number related to the warning.
 *  param warning - The warning message format string.
 *  ... - Additional arguments to format the warning message.
 */
void warnprintf(const char file_name[], const int line, const char *warning, ...) {
    va_list args;
    va_start(args, warning);

    fprintf(stdout, "File " ANSI_BOLD "'%s': " ANSI_STYLE_RESET "Line %d: "
            ANSI_BOLD ANSI_COLOR_PURPLE "Warning: " ANSI_STYLE_RESET, file_name, line);

    vfprintf(stdout, warning, args);
    
    fprintf(stdout, "\n");

    va_end(args);
}



/*
 * Checks the length of a line, ensuring it's within the allowed length.
 *
 * param line - Current line being checked.
 * Returns 1 if the line's length is within the limit, otherwise returns 0.
 */
int check_length(char *line){

	return strlen(line) < MAX_LINE;
	
}













