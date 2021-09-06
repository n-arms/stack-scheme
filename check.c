#include "check.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

bool cons_starts_with(expr *e, char *symbol) {
    return 
        e -> value.cons.values -> tag == SYMBOL &&
        (strcmp(e -> value.cons.values -> value.symbol.s, symbol) == 0);
}

bool is_quote(expr *e) {
    return
        e -> tag == CONS &&
        e -> value.cons.length == 2 &&
        cons_starts_with(e, "quote"); 
}

bool validate(expr *e) {
    if (e -> tag != CONS) return true;
    if (is_quote(e)) return true;
    for (int i = 0; i < e -> value.cons.length; ++i)
        if (!validate(e -> value.cons.values + i))
            return false;
    return true;
}
