assembler: pre_assembler.o data_structures.o funcs_and_macs.o assembler.o ast.o first_run.o encoder.o second_run.o base64.o
	gcc -g -Wall -ansi -pedantic pre_assembler.o data_structures.o assembler.o funcs_and_macs.o first_run.o encoder.o second_run.o base64.o ast.o -o assembler

pre_assembler.o: pre_assembler.c funcs_and_macs.h macro_list.h
	gcc -c -g -Wall -ansi -pedantic pre_assembler.c -o pre_assembler.o
	
data_structures.o: data_structures.c macro_list.h labels_BST.h funcs_and_macs.h
	gcc -c -g -Wall -ansi -pedantic data_structures.c -o data_structures.o
	
funcs_and_macs.o: funcs_and_macs.c funcs_and_macs.h
	gcc -c -g -Wall -ansi -pedantic funcs_and_macs.c -o funcs_and_macs.o
	
assembler.o: assembler.c assembler.h macro_list.h labels_BST.h funcs_and_macs.h ast.h encoder.h
	gcc -c -g -Wall -ansi -pedantic assembler.c -o assembler.o

ast.o: ast.c ast.h funcs_and_macs.h
	gcc -c -g -Wall -ansi -pedantic ast.c -o ast.o

first_run.o: first_run.c macro_list.h labels_BST.h ast.h funcs_and_macs.h
	gcc -c -g -Wall -ansi -pedantic first_run.c -o first_run.o

encoder.o: encoder.c labels_BST.h ast.h funcs_and_macs.h encoder.h
	gcc -c -g -Wall -ansi -pedantic encoder.c -o encoder.o

second_run.o: second_run.c labels_BST.h ast.h funcs_and_macs.h encoder.h
	gcc -c -g -Wall -ansi -pedantic second_run.c -o second_run.o
	
base64.o: base64.c encoder.h funcs_and_macs.h
	gcc -c -g -Wall -ansi -pedantic base64.c -o base64.o
	
	
