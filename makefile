all: shell.o
	gcc shell.o

shell.o: shell.c
	gcc -c shell.c

run:
	./a.out

clear:
	rm *.o
	rm *.out
	
