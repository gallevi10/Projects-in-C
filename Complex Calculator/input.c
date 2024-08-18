/*
 * This file contains functions that manages the input of the user.
 * Author: Gal Levi
 * Version: 19.05.22
*/


#include "complex.h"
#define ENLARGE_INPUT_SIZE (*size) += sizeof(char) /* macro to increase the input size of memory */
#define ENLARGE_PART_SIZE (*size) += sizeof(char*) /* macro to increase the partitions size of memory */
#define COMP_ARR_LEN 6 /* 6 complex variables */
#define ONE_PARAM 1 /* the supposed number of parameters of 1 parameter-command */
#define TWO_PARAMS 2
#define THREE_PARAMS 3

/* functions prototype */
void add_input(char **, int *, int *, char);
void check_str_malloc(char **, int);
void add_partition(char ***, int *, int *, char *);
void check_partition_malloc(char ***, int);
int check_double(char *);
void remove_spaces_tabs(char * );
int check_before_command(char *, char *);
int check_after_command_with_commas(char **, int);
void divide(char ***, char **, int *);
int check_comp_var(char *, char *);
int check_after_command_without_commas(char **);
int count_parameters(char *);
void check_2_param_comp_command(char *, char **, int , char ***, int *, command *, int *);
void check_2_param_comp_and_num_command(char *, char **, int , char ***, int *, command *, int *);
void check_1_param_command(char *, char **, int , command *, int *);

/* each command and its length */
enum commands_lengths {r_c_len = 9, p_c_len = 10, ad_c_len = 8, s_c_len = 8,
					 m_c_r_len = 14, m_c_i_len = 13, m_c_c_len = 14, ab_c_len = 8, s_len = 4};

/*
 * Parses user input and extracts the command details.
 *
 * Returns:
 *     If the command is valid, returns the parsed command details.
 *     If the command is invalid or empty, returns a default FAIL command.
 */
command get_command(){
	
	/* 
	 * command_size - command_input memory size
	 * input_length - the length of the input
	 * partitions_size - partitions memory size
	 * fail - flag to detect an invalid command
	 * j - loop pointer
	 * curr_char - current input character
	 * command_input - the input string
	 * com_ptr - pointer for analyzing the command
	 * partitions - array of the parts of the command which seperated by comma
	 */
	int command_size = 0, input_length = 0, partitions_size = 0, fail = 0, j;
	char curr_char;
	char *command_input, *com_ptr;
	char **partitions;
	const command FAIL = {0, '\0','\0', 0, 0}; /* the function returns it if the command is invalid/empty */
	
	command user_command = {0, '\0','\0', 0, 0}; /* initializes the function output with default values */
	
	/* memory allocation with checking */
	check_str_malloc(&command_input, command_size);
	check_partition_malloc(&partitions, partitions_size);
	
	/* receives input from user and continues to loop until new line is entered */
	while ((curr_char = getchar()) != '\n'){
		
		if (curr_char == EOF){ /* if user stops the program using EOF character */
			printf("ERROR - program terminated not by stop command\n");
			free(command_input);
			free(partitions);
			exit(0);
		} /* end of if */
		/* adds the new input char to curr_command array */
		add_input(&command_input, &input_length, &command_size, curr_char);
			
	} /* end of while loop */
	
	/* add null terminator to end of string */
	add_input(&command_input, &input_length, &command_size, '\0');
	
	printf("You have entered: %s\n", command_input);
	
	
	/*------------------------------------------case 1--------------------------------------*/
	
	/* if the command is read_comp assigns the pointer to start of command to com_ptr */
	if ( (com_ptr = strstr(command_input, "read_comp")) ){
	
		/* updates the kind of the command in the result output */
		user_command.com = r_c; /* r_c = 1 */
		
		if (!check_before_command(command_input, com_ptr)) /* checking the pre-command input */
			fail = 1;
		
		else { /* if check_before_command failed */
			com_ptr = com_ptr + r_c_len; /* com_ptr now points after the command, r_c_len = 9*/
		
			/* checking the post-command input and removes spaces and tabs*/
			if(!check_after_command_with_commas(&com_ptr, THREE_PARAMS))
				fail = 1;
			
			else{ /* if nothing failed */
				/* putting all the command parts which sepreated by comma in partitions array */
				divide(&partitions, &com_ptr, &partitions_size);
		
		
				/* checking the first partition */
		
				/* checks if there is a valid complex variable, if so assigns it to output result */
				if (!check_comp_var(partitions[0], &user_command.comp_var1))
				fail = 1;
		
				else{ /* if check_comp_var not failed */
					/* checking the second and the third partition which represents the complex numbers */
					for (j = 1; j <= 2; j++){
						if (check_double(partitions[j]) != 1)
							fail = 1;
					} /* end of for */
				} /* end of third else */
			} /* end of second else */
		} /* end of first else */
		
		if (!fail){
		
			/* assigns in the result output the parameters from string to float */
			user_command.num1 = atof(partitions[1]);
			user_command.num2 = atof(partitions[2]);
			
		} /* end of if */
		
		
		free(partitions);
		free(command_input);
		
		return !fail ? user_command : FAIL; /* if the command is valid returns the details, otherwise FAIL */
	
	} /* end of external if */
	
	/*------------------------------------------case 2--------------------------------------*/
	
	/* if the command is print_comp assigns the pointer to start of command to com_ptr */
	else if ( (com_ptr = strstr(command_input, "print_comp")) ){
	
		free(partitions); /* not using this here */
	
		user_command.com = p_c; /* p_c = 2 */
		
		/* analyzing the command and assigns the details in user_command if valid
		   p_c_len = 10 */
		check_1_param_command(command_input, &com_ptr, p_c_len, &user_command, &fail);
		
		
		return !fail ? user_command : FAIL; 
	
	} /* end of else if */
	
	/*------------------------------------------case 3--------------------------------------*/
	
	/* if the command is add_comp assigns the pointer to start of command to com_ptr */
	else if ( (com_ptr = strstr(command_input, "add_comp")) ){
	
		user_command.com = ad_c; /* ad_c = 3 */
		
		/* ad_c_len = 8 */
		check_2_param_comp_command(command_input, &com_ptr, ad_c_len, &partitions, 
									&partitions_size, &user_command, &fail);


		return !fail ? user_command : FAIL;
	
	} /* end of else if */
	
	/*------------------------------------------case 4--------------------------------------*/
	
	/* if the command is sub_comp assigns the pointer to start of command to com_ptr */
	else if ( (com_ptr = strstr(command_input, "sub_comp")) ){
	
		user_command.com = s_c; /* s_c = 4 */
		
		/* s_c_len = 8 */
		check_2_param_comp_command(command_input, &com_ptr, s_c_len, &partitions, 
									&partitions_size, &user_command, &fail);
		
		return !fail ? user_command : FAIL;
	
	} /* end of else if */
	
	/*------------------------------------------case 5--------------------------------------*/
	
	/* if the command is mult_comp_real assigns the pointer to start of command to com_ptr */
	else if ( (com_ptr = strstr(command_input, "mult_comp_real")) ){
	
		user_command.com = m_c_r; /* m_c_r = 5 */
		
		/* m_c_r_len = 14 */
		check_2_param_comp_and_num_command(command_input, &com_ptr, m_c_r_len, &partitions, 
									&partitions_size, &user_command, &fail);
									
		return !fail ? user_command : FAIL;
	
	} /* end of else if */
	
	/*------------------------------------------case 6--------------------------------------*/
	
	/* if the command is mult_comp_img assigns the pointer to start of command to com_ptr */
	else if ( (com_ptr = strstr(command_input, "mult_comp_img")) ){
	
		user_command.com = m_c_i; /* m_c_i = 6 */
		
		/* m_c_i_len = 13 */
		check_2_param_comp_and_num_command(command_input, &com_ptr, m_c_i_len, &partitions, 
									&partitions_size, &user_command, &fail);
									
		return !fail ? user_command : FAIL;
	
	} /* end of else if */
	
	/*------------------------------------------case 7--------------------------------------*/
	
	/* if the command is mult_comp_comp assigns the pointer to start of command to com_ptr */
	else if ( (com_ptr = strstr(command_input, "mult_comp_comp")) ){
	
		user_command.com = m_c_c; /* m_c_c = 7 */
		
		/* m_c_i_len = 14 */
		check_2_param_comp_command(command_input, &com_ptr, m_c_c_len, &partitions, 
									&partitions_size, &user_command, &fail);
									
		return !fail ? user_command : FAIL;
	
	} /* end of else if */
	
	/*------------------------------------------case 8--------------------------------------*/
	
	/* if the command is abs_comp assigns the pointer to start of command to com_ptr */
	else if ( (com_ptr = strstr(command_input, "abs_comp")) ){
	
		free(partitions); /* not using this here */
		
		user_command.com = ab_c; /* ab_c = 8 */
		

		/* ab_c_len = 8 */
		check_1_param_command(command_input, &com_ptr, ab_c_len, &user_command, &fail);
		
		return !fail ? user_command : FAIL;
	
	} /* end of else if */
	
	/*------------------------------------------case 9--------------------------------------*/
	
	/* if the command is stop assigns the pointer to start of command to com_ptr */
	else if ( (com_ptr = strstr(command_input, "stop")) ){
	
		user_command.com = s; /* s = 9 */
		
		if (!check_before_command(command_input, com_ptr)) /* checking the pre-command input */
			fail = 1;
		
		else{
			com_ptr = com_ptr + s_len; /* com_ptr now points after the command, s_len = 4 */
		
			/* removes spaces and tabs */
			remove_spaces_tabs(com_ptr);
	
			if (com_ptr[0] != '\0'){ /* if there is character after stop command */
				printf("Extraneous text after end of command\n");
				fail = 1;
			} /* end of if */
		} /* end of else */
		free(partitions);
		free(command_input);
		return !fail ? user_command : FAIL;
	} /* end of else if */

	/*------------------------------------------case 10-------------------------------------*/
	else{
		/* removes spaces and tabs */
		remove_spaces_tabs(command_input);
		if (*command_input != '\0') /* if input is not empty */
			printf("Undefined command name\n");
			
		free(partitions);
		free(command_input);
		return FAIL;
	} /* end of else */
	

} /* end of get_command function */


/*----------------------------------helper functions--------------------------------------------------*/


/*
 * This function checks a command with one parameter (a complex variable) for validity.
 * It performs checks on the pre-command input, post-command input, and the complex variable
 * parameter in the command. If any of the checks fail, the fail_flag is set to indicate failure.
 *
 * param command_input: The input command string.
 * param com_ptr: Pointer to the command string.
 * param command_len: The length of the command.
 * param user_command: Pointer to the user command struct.
 * param fail_flag: Pointer to the fail flag.
 */
void check_1_param_command(char *command_input, char **com_ptr, int command_len, command *user_command,
							int *fail_flag){


	/* checking the pre-command input */
	if (!check_before_command(command_input, *com_ptr)) 
			*fail_flag = 1;
		
		
	else{ /* if check_before_command not failed */
		*com_ptr = *com_ptr + command_len; /* skipping to the parameter area */
		
		
		/* checking the post-command input and removes spaces and tabs*/
		if (!check_after_command_without_commas(com_ptr))
			*fail_flag = 1;
		
		
		/* if the parameter is valid, assigns it in the output result */
		else if (!check_comp_var(*com_ptr, &(user_command -> comp_var1)))
			*fail_flag = 1;
				
	} /* end of else */
	
	/* frees the allocated memory */
	free(command_input);

} /* end of function check_1_param_command */


/*
 * This function checks a command with two parameters (one complex variable and one number) for validity.
 * It performs checks on the pre-command input, post-command input, and the complex variable and number
 * parameters in the command. If any of the checks fail, the fail_flag is set to indicate failure.
 *
 * param command_input: The input command string.
 * param com_ptr: Pointer to the command string.
 * param command_len: The length of the command.
 * param partitions: Pointer to the partitions array.
 * param partitions_size: Pointer to the size of the partitions array.
 * param user_command: Pointer to the user command struct.
 * param fail_flag: Pointer to the fail flag.
 */
void check_2_param_comp_and_num_command(char *command_input, char **com_ptr, int command_len, char ***partitions, 
							int *partitions_size, command *user_command, int *fail_flag){


	
	if (!check_before_command(command_input, *com_ptr)) /* checking the pre-command input */
		*fail_flag = 1;
	
	else{ /* if check_before_command not failed */
		*com_ptr = *com_ptr + command_len; /* com_ptr now points after the command */
		
		/* checking the post-command input */
		
		if(!check_after_command_with_commas(com_ptr, TWO_PARAMS))
			*fail_flag = 1;
			
		else{ /* if nothing failed */
		
			/* putting all the command parts which sepreated by comma in an array */
			divide(partitions, com_ptr, partitions_size);
			
			/* checks if there is a valid complex variable, if so assigns it to output result */
			if (!check_comp_var((*partitions)[0], &(user_command -> comp_var1)))
				*fail_flag = 1;
		
			/* checks the second number parameter */
			else if (check_double((*partitions)[1]) != 1)
				*fail_flag = 1;
			
			/* the second number is valid, assigns it to the result output */
			else
				user_command -> num1 = atof((*partitions)[1]);

		} /* end of inner else */
	} /* end of external else */
		
	/*frees the allocated memory */
	free(*partitions);
	free(command_input);

} /* end of function check_2_param_comp_and_num_command */



/*
 * This function checks a command with two parameters (two of them are complex variables) for validity.
 * It performs checks on the pre-command input, post-command input, and the complex variables
 * in the command. If any of the checks fail, the fail_flag is set to indicate
 * failure.
 * 
 * param command_input: The input command string.
 * param com_ptr: Pointer to the command string.
 * param command_len: The length of the command.
 * param partitions: Pointer to the partitions array.
 * param partitions_size: Pointer to the size of the partitions array.
 * param user_command: Pointer to the user command struct.
 * param fail_flag: Pointer to the fail flag.
 */
void check_2_param_comp_command(char *command_input, char **com_ptr, int command_len, char ***partitions, 
							int *partitions_size, command *user_command, int *fail_flag){

	if (!check_before_command(command_input, *com_ptr)) /* checking the pre-command input */
		*fail_flag = 1;
	
	else{ /* if check_before_command not failed */
		*com_ptr = *com_ptr + command_len; /* com_ptr now points after the command */
		
		/* checking the post-command input */
		
		if(!check_after_command_with_commas(com_ptr, TWO_PARAMS))
			*fail_flag = 1;
			
		else{ /* if nothing failed */
		
			/* putting all the command parts which sepreated by comma in an array */
			divide(partitions, com_ptr, partitions_size);
			
		
			/* checks if there is a valid complex variable, if so assigns it to output result */
			if (!check_comp_var((*partitions)[0], &(user_command -> comp_var1)) ||
				!check_comp_var((*partitions)[1], &(user_command -> comp_var2)))
				*fail_flag = 1;
		} /* end of inner else */
	} /* end of external else */
		
	/* frees the allocated memory */
	free(*partitions);
	free(command_input);

} /* end of function check_2_param_comp_command */




/*
 * This function takes a command string as input and counts the number of parameters
 * based on the occurrences of non-whitespace characters and commas.
 * 
 * param command: The input command string.
 * returns: The number of parameters in the command.
 */
int count_parameters(char *command){
	
	/* loop pointer, parameters counter and a helper flag to count the parameters */
	int i, parameters_cnt = 0, flag = 0;
	
	for (i = 0; command[i] != '\0'; i++){ /* until the end of the string */
		
		/* if the current character is not tab, space, comma */
		if (command[i] != '\t' && command[i] != ' ' && command[i] != ','){
			if (!flag){ /* if we have not count this parameter segment yet */
				flag = 1;
				parameters_cnt++;
			} /* end of inner if */
		} /* end of external if */
		else
			flag = 0; /* resets the flag because the current character is not a parameter value */
	
	} /* end of for */
	return parameters_cnt; /* returns the true number of parameters */

} /* end of function count_parameters */

/*
 * This function takes an input complex variable string and checks if it is valid by comparing it
 * against a list of the valid complex variables. If a match is found, the corresponding output variable
 * is assigned with the first character of the valid complex variable. The function returns 1 if the
 * input complex variable is valid, and returns 0 otherwise.
 * 
 * param input_comp: The input complex variable string.
 * param output_comp_var: A pointer to the output complex variable character.
 * returns: 1 if the input complex variable is valid, and returns 0 otherwise.
 */
int check_comp_var(char *input_comp, char *output_comp_var){

	int i; /* loop pointer */
	const char valid_comp_array[6][2] = {"A", "B", "C", "D", "E", "F"};
	
	for (i = 0; i < COMP_ARR_LEN; i++){ /* COMP_ARR_LEN = 6 */
		if (!strcmp (input_comp, valid_comp_array[i])){ /* if strcmp = 0 */
			*output_comp_var = valid_comp_array[i][0]; /* found a match, assigns it */
			break;
		} /* end of if */
	} /* end of for */
		
	/* we have not found a match - probably invalid complex variable */
	if (i == COMP_ARR_LEN){
		printf("Undefined complex variable\n");
		return 0;
	}

	return 1;

} /* end of function check_comp_var */


/*
 * This function takes a command string and divides it into partitions based on comma separators.
 * Each partition is added to a dynamic array of partitions. The function updates the length and
 * size of the partitions array accordingly. It uses the add_partition function to add each partition
 * to the array.
 * 
 * param partitions: A pointer to a pointer to the partitions array.
 * param com_ptr: A pointer to the command string.
 * param partitions_size: A pointer to the size of the partitions array.
 */
void divide(char ***partitions, char **com_ptr, int *partitions_size){
	
	int partitions_length = 0;
	char *command_parts = strtok(*com_ptr, ",");
	
	while(command_parts != NULL){
			
		add_partition(partitions, &partitions_length, partitions_size, command_parts);
		command_parts = strtok(NULL, ",");
		
	} /* end of while */

} /* end of function divide */

/*
 * This function checks if there are valid parameters after a specified command in a given string,
 * without the presence of commas. It also removes whitespace from the parameters. The function takes
 * a pointer to the command string as input.
 * 
 * param com_ptr: A pointer to the command string.
 * returns: 1 if the parameters are valid and properly formatted, and returns 0 otherwise.
 */
int check_after_command_without_commas(char **com_ptr){

	/* true number of parameters */
	int input_params_num = count_parameters(*com_ptr);
	
	
	if (input_params_num < ONE_PARAM){
		printf("Missing parameter\n");
		return 0;
	}
	
	if (count_parameters(*com_ptr) > ONE_PARAM){
		printf("Extraneous text after end of command\n");
		return 0;
	}
	
	if (strchr((*com_ptr), ',')){ /* if there is a comma in this command */
		printf("There should be no comma in this command\n");
		return 0;
	}
	
	/* if there is an invalid character right after the command */
	if (*com_ptr[0] != ' ' && *com_ptr[0] != '\t'){
		printf("Undefined command name\n");
		return 0;
	}
	

	
	/* all the the parameters are now without the valid white characters */
	remove_spaces_tabs(*com_ptr);
	
		
	return 1;

} /* end of function check_after_command_without_commas */

/*
 * This function checks if there are valid parameters after a specified command in a given string.
 * It also removes whitespace and commas from the parameters. The function takes a pointer to the
 * command string and the expected number of parameters as input.
 * 
 * param com_ptr: A pointer to the command string.
 * param supposed_params_num: The expected number of parameters.
 * returns: 1 if the parameters are valid and properly formatted, and returns 0 otherwise.
 */
int check_after_command_with_commas(char **com_ptr, int supposed_params_num){

	/* loop pointer, comma counter and the true number of parameters */
	int i;
	int comma_cnt = 0, input_params_num = count_parameters(*com_ptr);

	
	/* if there is an invalid character right after the command.
	   com_ptr[0] != ',' is for error priority */
	if (*com_ptr[0] != ' ' && *com_ptr[0] != '\t' && *com_ptr[0] != ','){
		printf("Undefined command name\n");
		return 0;
	}
	
	/* if there are less parameters than there should be */
	if (input_params_num < supposed_params_num){ 
		printf("Missing parameter\n");
		return 0;
	}
	
	/* if there are more parameters than there should be */
	if (count_parameters(*com_ptr) > supposed_params_num){
		printf("Extraneous text after end of command\n");
		return 0;
	}
	
		
	
	/* all the the parameters are now without the valid white characters */
	remove_spaces_tabs(*com_ptr);
		
	
	/* counts the commas in the command */
	for(i = 0; (*com_ptr)[i] != '\0'; i++)
		if ((*com_ptr)[i] == ',')
				comma_cnt++;
				
	/* in 2-parameter command supposed to be only 1 comma and in 3-parameter command supposed to be 2*/
	if (comma_cnt < (supposed_params_num - 1)){ 
			printf("Missing comma\n");
			return 0;
	}
	
	if (*com_ptr[0] == ','){ /* if there is an illegal comma right after the command */
		printf("Illegal comma\n");
		return 0;
	}
		
		
	if (strstr(*com_ptr,",,")){ /* if there is a double or more commas */
		printf("Multiple consecutive commas\n");
		return 0;
	}
	
			
	/* if the string ends with a comma */
	if (*(strrchr(*com_ptr, ',') + 1) == '\0'){ 
		
		if (comma_cnt > (supposed_params_num - 1)){ 
			printf("Extraneous text after end of command\n");
			return 0;
		} /* end of inner if */
		
	} /* end of external if */
	return 1;

} /* end of function check_after_command_with_commas */

/*
 * This function checks if there are any characters before a specified command
 * in a given string. It also validates if there are any invalid characters or
 * extraneous text before the start of the command. The function takes the command
 * input string and a pointer to the command within that string as input.
 * 
 * param command_input: A pointer to the command input string.
 * param com_ptr: A pointer to the start of the command within the command input string.
 * returns: 1 if there are no characters before the command or if they are valid,
 * and returns 0 if there are invalid characters or extraneous text before the command.
 */
int check_before_command(char* command_input, char* com_ptr){


	char *i = command_input; /* i is a loop (char *) pointer */
	
	if (i != com_ptr){ /* if there are characters before command */
	
		/* checks if there is an invalid character before the command */
		if (*(com_ptr - 1) != ' ' && *(com_ptr - 1) != '\t'){
			printf("Undefined command name\n");
			return 0;
		} /* end of if */
		
		while (i != com_ptr){
			if (*i != ' ' && *i != '\t'){
				printf("Extraneous text before start of command\n");
				return 0;
			} /* end of if */
			i = i + sizeof(char);
		} /* end of while */
		
	} /* end of external if */
	return 1;

} /* end of function check_before_command */


/*
 * This function removes spaces and tabs from a given string.
 * It modifies the original string by shifting the remaining characters to remove the spaces and tabs.
 * The resulting string does not contain any spaces or tabs.
 * 
 * param str: A pointer to the string to be modified.
 */
void remove_spaces_tabs(char* str) {

    char *temp = str;
    
    do {
        while (*temp == ' ' || *temp == '\t') {
        	++temp;
        } /* end of inner while */
        
    } while ((*str++ = *temp++));
} /* end of remove_spaces_tabs function */

/*
 * This function checks if a string represents a valid double value.
 * It takes a pointer to a string as input and performs the necessary
 * checks to determine if the string is a valid double.
 * 
 * param str: A pointer to a string.
 * return Returns: 1 if the string is a valid double, -1 if it is not a number,
 * or 0 if the parameter is missing.
 */
int check_double(char *str){
	
	/* loop poiner and a dot counter */
	int i, dot_cnt = 0;
	
	if (str == NULL){ /* the parameter is empty */
		printf("Missing parameter\n");
		return 0;
	} /* end of if */
		
	if (!strcmp(str, ".")){ /* the parameter is a dot */
		printf("Invalid parameter - not a number\n");
		return -1;
	} /* end of if */
		
	for (i = 0; i < strlen(str); i++){
		if (str[i] == '.' && dot_cnt == 0) /* the number is valid if it has only 1 dot */
			dot_cnt++;
			
		else if ((i == 0 && str[i] == '-')) /* if the number has a sign */
			continue;
			
		else if (!isdigit(str[i])){ /* if not number at all */
			printf("Invalid parameter - not a number\n");
			return -1;
		} /* end of else if */
	} /* end of for */
			
	return 1;

} /* end of function check_double */


/*
 * This function adds a new character to an array and dynamically reallocates memory if necessary.
 * It takes a double pointer to a character array, a pointer to the length of the array,
 * a pointer to the size of the array, and the new character to be added as input.
 * 
 * param a: A double pointer to a character array.
 * param length: A pointer to the length of the array.
 * param size: A pointer to the size of the array.
 * param newChar: The new character to be added.
 */
void add_input(char **a, int *length, int *size, char newChar){
	
	/* temporary pointer for dynamic reallocation check */
	char *temp = NULL;
	
	/* checks if there is enough memory for the next element and reallocate if necessary */
	if(!(temp = (char *)realloc(*a, ENLARGE_INPUT_SIZE))){ /* ENLARGE_SIZE is (*size) += sizeof(int) */
		printf("*ERROR* - there is not enough memory.\n");
		free(temp);
		exit(0);
	} /* end of if statement */
	else
		*a = temp; /* sets the new address of the memory block if successfully reallocated */
		        
	(*a)[(*length)++] = newChar; /* adds element and increases length */

} /* end of function add_input */

/*
 * This function adds a new partition (string) to a 2D array and dynamically reallocates
 * memory if necessary. It takes a triple pointer to a character array, a pointer to the
 * length of the array, a pointer to the size of the array, and a pointer to the new partition
 * as input.
 * 
 * param a: A triple pointer to a character array.
 * param length: A pointer to the length of the array.
 * param size: A pointer to the size of the array.
 * param newPart: A pointer to the new partition (string) to be added.
 */
void add_partition(char ***a, int *length, int *size, char *newPart){
	
	/* temporary pointer for dynamic reallocation check */
	char **temp = NULL;
	
	/* checks if there is enough memory for the next string and reallocate if necessary */
	if(!(temp = (char **)realloc(*a, ENLARGE_PART_SIZE))){ /* ENLARGE_PART_SIZE is (*size) += sizeof(char *) */
		printf("*ERROR* - there is not enough memory.\n");
		free(temp);
		exit(0);
	} /* end of if statement */
	else
		*a = temp; /* sets the new address of the memory block if successfully reallocated */
		        
	(*a)[(*length)++] = newPart; /* adds the string and increases length */

} /* end of function add_partition */


/*
 * This function dynamically allocates memory for a string.
 * It takes a double pointer to a character as input and updates its value
 * to point to the allocated memory.
 * The function terminates the program if there is not enough memory
 * available for allocation.
 *
 * param a: A double pointer to a character.
 * param size: The size of the string to be allocated.
 */
void check_str_malloc(char **a, int size){

	*a = (char *)malloc(size);
	
	if (!(*a)){ /* pointer is null */
		printf("Not enough memory, try again...");
		exit(0);
	}

} /* end of function check_str_malloc */

/*
 * This function dynamically allocates memory for a 2D array of characters.
 * It takes a triple pointer to a character array as input and updates its value
 * to point to the allocated memory.
 * The function terminates the program if there is not enough memory
 * available for allocation.
 * 
 * param a: A triple pointer to a character array.
 * param size: The size of the array to be allocated.
 *
 */
void check_partition_malloc(char ***a, int size){

	*a = (char **)malloc(size);
	
	if (!(*a)){ /* pointer is null */
		printf("Not enough memory, try again...");
		exit(0);
	}

} /* end of function check_partition_malloc */
