#include "compiler.h"
#include <stdlib.h>
#include <stdio.h>

op_chunk *compile(expr *e, int const_max) {
    op_chunk *result = new_op_chunk(const_max);
    int const_index = 0;
tailcall:
    switch (e -> tag) {
    case NUMBER:
        result -> constants[const_index].number.d = e -> value.number.d;
        add_op(result, LOAD_CONST_OP);
        add_op(result, const_index);
        ++ const_index;
        return result;
    case BOOLEAN:
        result -> constants[const_index].boolean.b = e -> value.boolean.b;
        add_op(result, LOAD_CONST_OP);
        add_op(result, const_index);
        ++ const_index;
        return result;
    case SYMBOL:
        result -> constants[const_index].heap_ref.ref = malloc(sizeof(heap_object));
        result -> constants[const_index].heap_ref.ref -> object_type = SYMBOL_OBJ;
        result -> constants[const_index].heap_ref.ref -> value.symbol.s = e -> value.symbol.s;
        add_op(result, LOAD_CONST_OP);
        add_op(result, const_index);
        ++ const_index;
        return result;
    case STRING:
        result -> constants[const_index].heap_ref.ref = malloc(sizeof(heap_object));
        result -> constants[const_index].heap_ref.ref -> object_type = STRING_OBJ;
        result -> constants[const_index].heap_ref.ref -> value.string.s = e -> value.string.s;
        add_op(result, LOAD_CONST_OP);
        add_op(result, const_index);
        ++ const_index;
        return result;
    case CONS:
        printf("not yet implemented compile(cons)\n");
        exit(1);
    }
}
