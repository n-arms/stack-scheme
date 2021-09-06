#include "check.h"
#include <stdlib.h>
#include <string.h>

bool is_quote(expr *e) {
    return
        e -> tag == CONS &&
        e -> value.cons.length == 2 &&
        e -> value.cons.values -> tag == SYMBOL &&
        (strcmp(e -> value.cons.values -> value.symbol.s, "quote") == 0);
}

bool is_define_or_set(expr *e) {
    return 
        e -> tag == CONS &&
        e -> value.cons.length == 3 &&
        e -> value.cons.values -> tag == SYMBOL &&
        e -> value.cons.values[1].tag == SYMBOL &&
        ((strcmp(e -> value.cons.values -> value.symbol.s, "define") == 0) ||
         (strcmp(e -> value.cons.values -> value.symbol.s, "set!") == 0));
}

bool is_if(expr *e) {
    return
        e -> tag == CONS &&
        e -> value.cons.length == 4 &&
        e -> value.cons.values -> tag == SYMBOL &&
        (strcmp(e -> value.cons.values -> value.symbol.s, "if") == 0);
}

bool validate(expr *e) {
tailcall:
    if (e -> tag != CONS) return true;
    if (is_quote(e)) return true;
    for (int i = 1; i < e -> value.cons.length; ++i)
        if (!validate(e -> value.cons.values + i))
            return false;
    if (e -> value.cons.values -> tag != SYMBOL) {
        e = e -> value.cons.values;
        goto tailcall;
    }

    return is_if(e) || is_define_or_set(e);
}
