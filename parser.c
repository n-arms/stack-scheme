#include "parser.h"
#include <stdlib.h>
#include <stdio.h>

expr *parse_num(token_buffer *tb) {
    if (tb -> buffer[tb -> index].tag != TOKEN_NUMBER ||
            (tb -> index + 1) > tb -> size)
        return NULL;
    expr * result = malloc(sizeof(expr));
    result -> tag = NUMBER;
    result -> value.number.d = tb -> buffer[tb -> index].value.number.d;
    ++ tb -> index;
    return result;
}

expr *parse_true(token_buffer *tb) {
    if (tb -> buffer[tb -> index].tag != TOKEN_TRUE ||
            (tb -> index + 1) > tb -> size)
        return NULL;
    expr * result = malloc(sizeof(expr));
    result -> tag = BOOLEAN;
    result -> value.boolean.b = 1;
    ++ tb -> index;
    return result;
}

expr *parse_false(token_buffer *tb) {
    if (tb -> buffer[tb -> index].tag != TOKEN_FALSE ||
            (tb -> index + 1) > tb -> size)
        return NULL;
    expr * result = malloc(sizeof(expr));
    result -> tag = BOOLEAN;
    result -> value.boolean.b = 0;
    ++ tb -> index;
    return result;
}

expr *parse_sym(token_buffer *tb) {
    if (tb -> buffer[tb -> index].tag != TOKEN_SYMBOL)
        return NULL;
    expr *result = malloc(sizeof(expr));
    result -> tag = SYMBOL;
    result -> value.symbol.s = tb -> buffer[tb -> index].value.symbol.s;
    ++ tb -> index;
    return result;
}

expr *parse_str(token_buffer *tb) {
    if (tb -> buffer[tb -> index].tag != TOKEN_STRING)
        return NULL;
    expr *result = malloc(sizeof(expr));
    result -> tag = STRING;
    result -> value.string.s = tb -> buffer[tb -> index].value.string.s;
    ++ tb -> index;
    return result;
}

expr *parse_char(token_buffer *tb) {
    printf("not yet implemented lexing for chars\n");
    exit(1);
}

expr *parse_nil(token_buffer *tb) {
    if ((tb -> index + 2) > tb -> size ||
            tb -> buffer[tb -> index].tag != TOKEN_LPAR ||
            tb -> buffer[tb -> index + 1].tag != TOKEN_RPAR)
        return NULL;
    expr *result = malloc(sizeof(expr));
    result -> tag = NIL;
    tb -> index += 2;
    return result;
}

expr *make_list(expr **values, int length) {
    if (length <= 0) return NULL;
    if (length == 1) return values[0];
    expr *result;
    result = malloc(sizeof(expr));
    result -> tag = PAIR;
    result -> value.pair.car = values[0];
    result -> value.pair.cdr = make_list(values + 1, length - 1);
    return result;
}

#define LIST_BUFFER 32
expr *parse_list(token_buffer *tb) {
    if ((tb -> index + 3) > tb -> size ||
            tb -> buffer[tb -> index].tag != TOKEN_LPAR)
        return NULL;
    ++ tb -> index;
    expr **result = malloc(sizeof(expr*) * LIST_BUFFER);
    result[0] = parse_expr(tb);
    if (result[0] == NULL) {
        free(result);
        return NULL;
    }
    int i;
    for (i = 1;; ++i) {
        result[i] = parse_expr(tb);
        if (result[i] == NULL) break;
    }
    if (tb -> buffer[tb -> index].tag == TOKEN_DOT) {
        ++ tb -> index;
        result[i] = parse_expr(tb);
        return make_list(result, i + 1);
    } else if (tb -> buffer[tb -> index].tag == TOKEN_RPAR) {
        ++ tb -> index;
        result[i] = malloc(sizeof(expr*));
        result[i] -> tag = NIL;
        return make_list(result, i + 1);
    } else {
        return NULL;
    }
}

expr *parse_quoted(token_buffer *tb) {
    if (tb -> buffer[tb -> index].tag != TOKEN_QUOTE)
        return NULL;
    ++ tb -> index;
    expr *result = malloc(sizeof(expr));
    result -> tag = PAIR;
    result -> value.pair.car = malloc(sizeof(expr));
    result -> value.pair.car -> tag = SYMBOL;
    result -> value.pair.car -> value.symbol.s = "quote";
    result -> value.pair.cdr = malloc(sizeof(expr));
    result -> value.pair.cdr -> tag = PAIR;
    result -> value.pair.cdr -> value.pair.cdr = malloc(sizeof(expr));
    result -> value.pair.cdr -> value.pair.cdr -> tag = NIL;
    result -> value.pair.cdr -> value.pair.car = parse_expr(tb);
    if (result -> value.pair.cdr) return result;
    printf("could not parse expression after parsing quote\n");
    return NULL;
}

expr *parse_expr(token_buffer *tb) {
    expr *result;
    result = parse_num(tb);
    if (result) return result;
    result = parse_sym(tb);
    if (result) return result;
    result = parse_str(tb);
    if (result) return result;
    result = parse_nil(tb);
    if (result) return result;
    result = parse_true(tb);
    if (result) return result;
    result = parse_false(tb);
    if (result) return result;
    result = parse_list(tb);
    if (result) return result;
    result = parse_quoted(tb);
    if (result) return result;
    return NULL;
}
