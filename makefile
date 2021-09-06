run: a.out
	./a.out

a.out: main.c expr.o lexer.o parser.o op.o check.o compiler.o
	gcc main.c expr.o lexer.o parser.o op.o check.o compiler.o

expr.o: expr.c expr.h
	gcc -c expr.c

lexer.o: lexer.c lexer.h
	gcc -c lexer.c

parser.o: parser.c parser.h
	gcc -c parser.c

op.o: op.c op.h
	gcc -c op.c

check.o: check.c check.h
	gcc -c check.c

compiler.o: compiler.c compiler.h
	gcc -c compiler.c
