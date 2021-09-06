#include "parser.h"
#include <stdlib.h>
#include <stdio.h>

#define LIST_BUFFER 32

expr *parse_atom(token_buffer *tb) {
    expr *result = malloc(sizeof(expr));
    switch (tb -> buffer[tb -> index].tag) {
    case TOKEN_NUMBER:
        result -> tag = NUMBER;
        result -> value.number.d = tb -> buffer[tb -> index].value.number.d;
        break;
    case TOKEN_TRUE:
        result -> tag = BOOLEAN;
        result -> value.boolean.b = 1;
        break;
    case TOKEN_FALSE:
        result -> tag = BOOLEAN;
        result -> value.boolean.b = 0;
        break;
    case TOKEN_STRING:
        result -> tag = STRING;
        result -> value.string.s = tb -> buffer[tb -> index].value.string.s;
        break;
    case TOKEN_SYMBOL:
        result -> tag = SYMBOL;
        result -> value.symbol.s = tb -> buffer[tb -> index].value.symbol.s;
        break;
    default:
        free(result);
        return NULL;
    }
    ++ tb -> index;
    return result;
}

expr *parse_list(token_buffer *tb) {
    if (tb -> buffer[tb -> index].tag != TOKEN_LPAR) return NULL;

    expr *values = malloc(sizeof(expr) * LIST_BUFFER);
    int length = 0;
    ++ tb -> index;

    while (tb -> buffer[tb -> index].tag != TOKEN_RPAR) {
        values[length] = *parse_expr(tb);
        ++length;
    }
    ++ tb -> index;

    expr *result = malloc(sizeof(expr));
    result -> tag = CONS;
    result -> value.cons.length = length;
    result -> value.cons.values = values;
    return result;
}

expr *parse_expr(token_buffer *tb) {
    if (tb -> buffer[tb -> index].tag == TOKEN_QUOTE) {
        ++ tb -> index;
        expr *result = malloc(sizeof(expr));
        result -> tag = CONS;
        result -> value.cons.length = 2;
        result -> value.cons.values = malloc(sizeof(expr) * 2);
        result -> value.cons.values -> tag = SYMBOL;
        result -> value.cons.values -> value.symbol.s = "quote";
        expr *contents = parse_list(tb);
        if (!contents)
            contents = parse_atom(tb);
        if (!contents) return NULL;
        result -> value.cons.values[1] = *contents;
        return result;
    } else {
        expr *result = parse_list(tb);
        if (result) return result;
        result = parse_atom(tb);
        return result;
    }
}
