#include "expr.h"
#include <stdlib.h>
#include <stdio.h>

token_buffer *new_token_buffer() {
    token_buffer *tb = malloc(sizeof(token_buffer));
    tb -> index = 0;
    tb -> size = 0;
    tb -> capacity = 1;
    tb -> buffer = malloc(sizeof(token));
    return tb;
}

void extend_token_buffer(token_buffer *tb) {
    if (tb -> capacity == tb -> size) {
        token *new_buffer = malloc(sizeof(token) * tb -> capacity * 2);
        for (int i = 0; i < tb -> size; ++i)
            new_buffer[i] = tb -> buffer[i];
        tb -> buffer = new_buffer;
        tb -> capacity *= 2;
    }
    ++ tb -> size;
}

void print_token(token *t) {
    switch (t -> tag) {
    case TOKEN_LPAR:
        printf("(");
    case TOKEN_RPAR:
        printf(")");
    case TOKEN_TRUE:
        printf("#t");
    case TOKEN_FALSE:
        printf("#f");
    case TOKEN_NUMBER:
        printf("%f", t -> value.number.d);
    case TOKEN_STRING:
        printf("%s", t -> value.string.s);
    case TOKEN_SYMBOL:
        printf("%s", t -> value.symbol.s);
    case TOKEN_QUOTE:
        printf("'");
    }
}

void print_tokens(token_buffer *tb) {
    for (int i = 0; i < tb -> size; ++i) {
        print_token(tb -> buffer + i);
        printf(" ");
    }
    printf("\n");
}
