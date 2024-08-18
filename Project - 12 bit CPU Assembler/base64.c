/*
 *	File: base64.c
 *
 *	Functions for encoding code and data into base64 representation.
 *
 *	author: Gal Levi
 *	version: 5.8.23
 */


#include "encoder.h"
#include "funcs_and_macs.h"


#define BASE_LINE_LEN 3 /* length of the base64 encoded line (2 characters + null terminator) */
#define LAST_SIX_BITS_MASK 0x03F /* bitmask to extract the last 6 bits of a number */
#define FIRST_SIX_BITS_MASK 0xFC0 /* bitmask to extract the first 6 bits of a number */

/* exclusive functions prototype */
void code_word_to_base64(mem_code_word, char *);
void num_to_base64(unsigned int, char *);



/*
 *	Exports the encoded code and data into a base64-encoded output file.
 *
 *	param file_name - The name of the source file being processed.
 *	param code_im - Pointer to the code image memory buffer.
 *	param ic - The instruction counter indicating the number of instructions.
 *	param data_im - Pointer to the data image memory buffer.
 *	param dc - The data counter indicating the number of data entries.
 */
void export_code_and_data_in_base64(char *file_name, mem_code_word (*code_im)[MAX_BUFFER], int ic,
									 mem_data_word (*data_im)[MAX_BUFFER], int dc){
	FILE *ob_des;
	char curr_base_line[BASE_LINE_LEN], ob_name[MAX_BUFFER];
	int i;
	
	sprintf(ob_name, "%s.ob",file_name);
	curr_base_line[BASE_LINE_LEN - 1] = '\0'; /* inserts null terminator to string */
	
	if (!(ob_des = fopen(ob_name, "w"))){
	
		errprintf(ob_name, NO_LINE_ERROR, "cannot write file");
		return;
		
	}
	
	fprintf(ob_des , "%d %d\n", ic, dc); /* headline */
	
	/* prints code to .ob file */
	for (i = 0; i < ic; i++){
		code_word_to_base64((*code_im)[i], curr_base_line);
        fprintf(ob_des, "%s\n", curr_base_line);
	}
	
	/* prints data to .ob file */
	for (i = 0; i < dc; i++){
		num_to_base64((*data_im)[i].curr_data, curr_base_line);
        fprintf(ob_des ,"%s\n", curr_base_line);
	}

	fclose(ob_des);

}





/*
 *	Converts a mem_code_word into a base64 encoded string.
 *
 *	param code_word - The memory code word to be encoded.
 *	param curr_base_line - Pointer to the buffer where the base64 encoded line will be stored.
 */
void code_word_to_base64(mem_code_word code_word, char *curr_base_line){
	
	/* *((unsigned int *)&(<memory word>)) = memory word as a number */
	switch (code_word.word_type){
		
		/* if it is first word */
		case first_word: num_to_base64(*((unsigned int *)&(code_word.union_word.first_word)), curr_base_line); 
		break;
		
		/* if it is label or number */
		case imm_dir_word: num_to_base64(*((unsigned int *)&(code_word.union_word.imm_direct_add_word)), curr_base_line);
		break;
		
		case reg_word: num_to_base64(*((unsigned int *)&(code_word.union_word.reg_add_word)), curr_base_line);
	}

}


/*
 *	Converts an unsigned integer into its base64 encoded representation.
 *
 *	param num - The unsigned integer to be encoded.
 *	param curr_base_line - Pointer to the buffer where the base64 encoded line will be stored.
 */
void num_to_base64(unsigned int num, char *curr_base_line) {
	
	/* every index is the code of the relevant character in the BASE64 table */
	const char BASE64_TABLE[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    /* inserts 6 first bits of num */
    curr_base_line[0] = BASE64_TABLE[((num & FIRST_SIX_BITS_MASK) >> 6)];
    /* inserts 6 last bits of num */
    curr_base_line[1] = BASE64_TABLE[(num & LAST_SIX_BITS_MASK)];
	
}






