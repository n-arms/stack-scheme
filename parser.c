#include "parser.h"
#include <stdlib.h>

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
    return NULL;
}

expr *parse_expr(token_buffer *tb) {
    return parse_atom(tb);
}
