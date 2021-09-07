#ifndef EXPR_H_
#define EXPR_H_

#include <stdbool.h>

typedef enum {
    TOKEN_NUMBER,
    TOKEN_LPAR,
    TOKEN_RPAR,
    TOKEN_TRUE,
    TOKEN_FALSE,
    TOKEN_STRING,
    TOKEN_SYMBOL,
    TOKEN_QUOTE,
    TOKEN_DOT
} token_tag;

typedef struct {
    token_tag tag;
    union {
        struct {
            double d;
        } number;
        struct {
            char *s;
        } string;
        struct {
            char *s;
        } symbol;
    } value;
} token;

typedef struct {
    int size;
    int index;
    int capacity;
    token *buffer;
} token_buffer;

token_buffer *new_token_buffer();
void extend_token_buffer(token_buffer *tb);

void print_token(token *t);
void print_tokens(token_buffer *tb);

typedef enum {
    NUMBER,
    SYMBOL,
    STRING,
    PAIR,
    CHAR,
    BOOLEAN,
    NIL
} expr_tag;

typedef struct expr {
    expr_tag tag;
    union {
        struct {
            double d;
        } number;
        struct {
            char *s;
        } symbol;
        struct {
            char *s;
        } string;
        struct {
            struct expr *car;
            struct expr *cdr;
        } pair;
        struct {
            char c;
        } character;
        struct {
            bool b;
        } boolean;
    } value;
} expr;

void print_expr(expr *e);

bool is_valid(expr *e);

#endif
