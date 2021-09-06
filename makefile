run: a.out
	./a.out

a.out: main.c expr.o lexer.o parser.o op.o
	gcc main.c expr.o lexer.o parser.o op.o

expr.o: expr.c expr.h
	gcc -c expr.c

lexer.o: lexer.c lexer.h
	gcc -c lexer.c

parser.o: parser.c parser.h
	gcc -c parser.c

op.o: op.c op.h
	gcc -c op.c
