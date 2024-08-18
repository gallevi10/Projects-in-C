mycomp: mycomp.o complex.o input.o
	gcc -g -Wall -ansi -pedantic mycomp.o complex.o input.o -o mycomp -lm

mycomp.o: mycomp.c complex.h
	gcc -c -g -Wall -ansi -pedantic mycomp.c -o mycomp.o
	
complex.o: complex.c complex.h
	gcc -c -g -Wall -ansi -pedantic complex.c -o complex.o

input.o: input.c complex.h
	gcc -c -g -Wall -ansi -pedantic input.c -o input.o

