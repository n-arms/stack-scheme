#include "compiler.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

/*
void insert(op_chunk *a, op_chunk *b) {
    for (int i = 0; i < b -> size; ++i)
        add_op(a, b -> data[i]);
    for (int i = 0; i < b -> constants -> size; ++i)
        add_const(a -> constants, b -> constants -> constants[i]);
}*/

bool operator_is(expr *e, char *s) {
    assert(e -> tag == CONS);
    assert(e -> value.cons.values -> tag == SYMBOL);
    return strcmp(e -> value.cons.values -> value.symbol.s, s) == 0;
}

int num_operands(expr *e) {
    assert(e -> tag == CONS);
    return e -> value.cons.length;
}

stack_object compile_number(expr *e) {
    stack_object num;
    num.number.d = e -> value.number.d;
    return num;
}

stack_object compile_boolean(expr *e) {
    assert(e -> tag == BOOLEAN);
    stack_object boolean;
    boolean.boolean.b = e -> value.boolean.b;
    return boolean;
}

stack_object compile_symbol(expr *e) {
    assert(e -> tag == SYMBOL);
    stack_object sym;
    sym.heap_ref.ref = malloc(sizeof(heap_object));
    sym.heap_ref.ref -> object_type = SYMBOL_OBJ;
    sym.heap_ref.ref -> value.symbol.s = e -> value.symbol.s;
    return sym;
}

stack_object compile_string(expr *e) {
    assert(e -> tag == STRING);
    stack_object str;
    str.heap_ref.ref = malloc(sizeof(heap_object));
    str.heap_ref.ref -> object_type = STRING_OBJ;
    str.heap_ref.ref -> value.string.s = e -> value.string.s;
    return str;
}

stack_object compile_quoted(expr *e);
stack_object compile_list(expr *e) {
    assert(e -> tag == CONS);
    stack_object list;
    list.heap_ref.ref = malloc(sizeof(heap_object));
    list.heap_ref.ref -> object_type = LIST_OBJ;
    list.heap_ref.ref -> value.cons.length = e -> value.cons.length;
    list.heap_ref.ref -> value.cons.values = malloc(sizeof(stack_object) * e -> value.cons.length);
    for (int i = 0; i < e -> value.cons.length; ++i)
        list.heap_ref.ref -> value.cons.values[i] = compile_quoted(e -> value.cons.values + i);
    return list;
}

stack_object compile_quoted(expr *e) {
    switch (e -> tag) {
    case NUMBER:
        return compile_number(e);
    case BOOLEAN:
        return compile_boolean(e);
    case STRING:
        return compile_string(e);
    case SYMBOL:
        return compile_symbol(e);
    case CONS:
        return compile_list(e);
    }
}

void load_const(stack_object o, op_chunk *target) {
    add_const(target -> constants, o);
    add_op(target, LOAD_CONST_OP);
    add_op(target, target -> constants -> size - 1);
}

void compile_if(expr *e, op_chunk *target) {
    assert(e -> tag == CONS);
    assert(e -> value.cons.length == 4);
    assert(e -> value.cons.values -> tag == SYMBOL);
    assert(strcmp(e -> value.cons.values -> value.symbol.s, "if") == 0);
}

void compile(expr *e, op_chunk *target) {
tailcall:
    switch (e -> tag) {
    case NUMBER:
        return load_const(compile_number(e), target);
    case BOOLEAN:
        if (e -> value.boolean.b) {
            add_op(target, LOAD_TRUE_OP);
        } else {
            add_op(target, LOAD_FALSE_OP);
        }
        return;
    case SYMBOL:
        return load_const(compile_symbol(e), target);
    case STRING:
        return load_const(compile_string(e), target);
    case CONS:
        if (e -> value.cons.length == 0) {
            printf("cannot evaluate the empty list\n");
            return;
        }
        if (e -> value.cons.values -> tag != SYMBOL) {
            printf("not yet implemented higher order functions\n");
            exit(1);
        }
        /*
        if (strcmp(e -> value.cons.values -> value.symbol.s, "quote") == 0) {
            return load_const(compile_quoted(e), target);
        } else if (strcmp(e -> value.cons.values -> value.symbol.s, "if") == 0) {
        } else if (strcmp(e -> value.cons.values -> value.symbol.s, "define") == 0) {
        } else if (strcmp(e -> value.cons.values -> value.symbol.s, "set!") == 0) {
        } else {
        }*/
        if (operator_is(e, "quote") && num_operands(e) == 2) {
            return load_const(compile_quoted(e), target);
        } else if (operator_is(e, "if") && num_operands(e) == 4) {
            return compile_if(e, target);
        } else if (operator_is(e, "define") && num_operands(e) == 3) {
        } else if (operator_is(e, "set!") && num_operands(e) == 3) {
        } else if (operator_is(e, "/") && num_operands(e) == 3) {
            compile(e -> value.cons.values + 1, target);
            compile(e -> value.cons.values + 2, target);
            return add_op(target, DIV_OP);
        } else if (operator_is(e, "+")) {
            load_const((stack_object){{0.0}}, target);
            for (int i = 1; i < num_operands(e); ++i) {
                compile(e -> value.cons.values + i, target);
                add_op(target, ADD_OP);
            }
            return;
        } else if (operator_is(e, "*")) {
            load_const((stack_object){{1.0}}, target);
            for (int i = 1; i < num_operands(e); ++i) {
                compile(e -> value.cons.values + i, target);
                add_op(target, MUL_OP);
            }
            return;
        } else if (operator_is(e, "-")) {
            if (num_operands(e) == 1) {
                fprintf(stderr, "cannot apply - to 0 operands");
            } else if (num_operands(e) == 2) {
                load_const((stack_object){{0.0}}, target);
                compile(e -> value.cons.values + 1, target);
                add_op(target, SUB_OP);
            } else {
                compile(e -> value.cons.values + 1, target);
                for (int i = 2; i < num_operands(e); ++i) {
                    compile(e -> value.cons.values + i, target);
                    add_op(target, SUB_OP);
                }
            }
        }
    }
}
