fibon: fibon.o circular_list.o fib_stack.o fib_functions.o
	gcc -g -Wall -ansi -pedantic fibon.o circular_list.o fib_stack.o fib_functions.o -o fibon

fibon.o: fibon.c fibon.h
	gcc -c -g -Wall -ansi -pedantic fibon.c -o fibon.o
	
circular_list.o: circular_list.c list.h
	gcc -c -g -Wall -ansi -pedantic circular_list.c -o circular_list.o

fib_stack.o: fib_stack.c fib_stack.h list.h
	gcc -c -g -Wall -ansi -pedantic fib_stack.c -o fib_stack.o

fib_functions.o: fib_functions.c fibon.h
	gcc -c -g -Wall -ansi -pedantic fib_functions.c -o fib_functions.o
