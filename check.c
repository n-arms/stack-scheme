#include "check.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

bool pair_starts_with(expr *e, char *symbol) {
    return 
        e -> value.pair.car -> tag == SYMBOL &&
        (strcmp(e -> value.pair.car -> value.symbol.s, symbol) == 0);
}

bool is_quote(expr *e) {
    return
        e -> value.pair.car -> tag == SYMBOL &&
        pair_starts_with(e, "quote") &&
        e -> value.pair.cdr -> tag == PAIR &&
        e -> value.pair.cdr -> value.pair.cdr -> tag == NIL;
}

bool validate(expr *e) {
    if (e -> tag != PAIR || is_quote(e)) 
        return true;
    return 
        validate(e -> value.pair.car) &&
        validate(e -> value.pair.cdr);
}
