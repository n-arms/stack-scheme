run: a.out
	./a.out

a.out: main.o expr.o lexer.o
	gcc main.o expr.o lexer.o

main.o: main.c
	gcc -c main.c

expr.o: expr.c expr.h
	gcc -c expr.c

lexer.o: lexer.c lexer.h
	gcc -c lexer.c
