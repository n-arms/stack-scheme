run: a.out
	./a.out

a.out: main.c expr.o lexer.o parser.o
	gcc main.c expr.o lexer.o parser.o

expr.o: expr.c expr.h
	gcc -c expr.c

lexer.o: lexer.c lexer.h
	gcc -c lexer.c

parser.o: parser.c parser.h
	gcc -c parser.c
