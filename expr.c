#include "expr.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

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
    assert(t);
    switch (t -> tag) {
    case TOKEN_LPAR:
        printf("(");
        break;
    case TOKEN_RPAR:
        printf(")");
        break;
    case TOKEN_TRUE:
        printf("#t");
        break;
    case TOKEN_FALSE:
        printf("#f");
        break;
    case TOKEN_NUMBER:
        printf("%f", t -> value.number.d);
        break;
    case TOKEN_STRING:
        printf("\"%s\"", t -> value.string.s);
        break;
    case TOKEN_SYMBOL:
        printf("%s", t -> value.symbol.s);
        break;
    case TOKEN_QUOTE:
        printf("'");
        break;
    case TOKEN_DOT:
        printf(".");
        break;
    }
}

void print_tokens(token_buffer *tb) {
    assert(tb);
    for (int i = 0; i < tb -> size; ++i) {
        print_token(tb -> buffer + i);
    }
}

bool is_valid(expr *e) {
    if (!e) return false;
    if (e -> tag == PAIR) {
        return is_valid(e -> value.pair.car) && is_valid(e -> value.pair.cdr);
    }
    return true;
}

void print_expr(expr *e) {
    assert(e);
    switch(e -> tag) {
    case NUMBER:
        printf("%f", e -> value.number.d);
        break;
    case BOOLEAN:
        if (e -> value.boolean.b) {
            printf("#t");
        } else {
            printf("#f");
        }
        break;
    case SYMBOL:
        printf("%s", e -> value.symbol.s);
        break;
    case STRING:
        printf("\"%s\"", e -> value.string.s);
        break;
    case PAIR:
        printf("( ");
        print_expr(e -> value.pair.car);
        printf(" . ");
        print_expr(e -> value.pair.cdr);
        printf(")");
        break;
    case CHAR:
        printf("%c", e -> value.character.c);
    case NIL:
        printf("()");
    }
}
