all: shell.o execute.o parse.o
	gcc shell.o parse.o execute.o shell.h

shell.o: shell.c
	gcc -c shell.c shell.h

execute.o: execute.c
	gcc -c execute.c shell.h

parse.o: parse.c
	gcc -c parse.c shell.h

run:
	./a.out

clear:
	rm *.o
	rm *.gch
	rm *.out
