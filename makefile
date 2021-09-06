run: a.out
	./a.out

a.out: main.c expr.o
	gcc main.c expr.o

expr.o: expr.c expr.h
	gcc -c expr.c
