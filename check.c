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

bool is_define_or_set(expr *e) {
    return 
        e -> tag == CONS &&
        e -> value.cons.length == 3 &&
        e -> value.cons.values[1].tag == SYMBOL &&
        (cons_starts_with(e, "define") ||
         cons_starts_with(e, "set!"));
}

bool is_if(expr *e) {
    return
        e -> tag == CONS &&
        e -> value.cons.length == 4 &&
        cons_starts_with(e, "if");
}

bool validate(expr *e) {
    if (e -> tag != CONS) return true;
    if (is_quote(e)) return true;
    for (int i = 1; i < e -> value.cons.length; ++i)
        if (!validate(e -> value.cons.values + i))
            return false;
    return 
        is_if(e) || 
        is_define_or_set(e) ||
        cons_starts_with(e, "+") ||
        cons_starts_with(e, "-") ||
        cons_starts_with(e, "*") ||
        (e -> value.cons.length == 3 && cons_starts_with(e, "/"));
}
