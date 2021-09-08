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

bool is_valid_list(expr *e) {
    return 
        e -> tag == PAIR &&
        (e -> value.pair.cdr -> tag == NIL ||
         is_valid_list(e -> value.pair.cdr));
}

bool operator_is(expr *e, char *s) {
    assert(is_valid_list(e));
    assert(e -> value.pair.car -> tag == SYMBOL);
    return strcmp(e -> value.pair.car -> value.symbol.s, s) == 0;
}

int num_operands(expr *e) {
    assert(is_valid_list(e));
    if (e -> value.pair.cdr -> tag == NIL) return 1;
    return 1 + num_operands(e -> value.pair.cdr);
}

expr *get_at(expr *e, int index) {
    assert(is_valid_list(e));
    if (index == 0) return e -> value.pair.car;
    return get_at(e -> value.pair.cdr, index - 1);
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

stack_object compile_nil(expr *e) {
    assert(e -> tag == NIL);
    return (stack_object){};
}

stack_object compile_char(expr *e) {
    assert(e -> tag == CHAR);
    stack_object character;
    character.character.c = e -> value.character.c;
    return character;
}

stack_object compile_quoted(expr *e);
stack_object compile_quoted_pair(expr *e) {
    /*
    stack_object list;
    list.heap_ref.ref = malloc(sizeof(heap_object));
    list.heap_ref.ref -> object_type = LIST_OBJ;
    list.heap_ref.ref -> value.cons.length = e -> value.cons.length;
    list.heap_ref.ref -> value.cons.values = malloc(sizeof(stack_object) * e -> value.cons.length);
    for (int i = 0; i < e -> value.cons.length; ++i)
        list.heap_ref.ref -> value.cons.values[i] = compile_quoted(e -> value.cons.values + i);
    return list;
    */
    stack_object list;
    list.heap_ref.ref = malloc(sizeof(heap_object));
    list.heap_ref.ref -> object_type = PAIR_OBJ;
    list.heap_ref.ref -> value.pair.car = malloc(sizeof(stack_object));
    list.heap_ref.ref -> value.pair.car[0] = compile_quoted(e -> value.pair.car);
    list.heap_ref.ref -> value.pair.cdr = malloc(sizeof(stack_object));
    list.heap_ref.ref -> value.pair.cdr[0] = compile_quoted(e -> value.pair.car);
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
    case CHAR:
        return compile_char(e);
    case NIL:
        return compile_nil(e);
    case PAIR:
        return compile_quoted_pair(e);
    }
}

void load_const(stack_object o, op_chunk *target) {
    add_const(target -> constants, o);
    add_op(target, LOAD_CONST_OP);
    add_op(target, target -> constants -> size - 1);
}

uint8_t *to_bytes(int i) {
    uint8_t *bytes = malloc(sizeof(uint8_t) * 4);
    bytes[0] = (uint8_t) (i >> 24);
    bytes[1] = (uint8_t) (i >> 16);
    bytes[2] = (uint8_t) (i >> 8);
    bytes[3] = (uint8_t) i;
    return bytes;
}

int from_bytes(uint8_t *bytes) {
    return ((((int) bytes[0]) << 24) + (((int) bytes[1]) << 16) + (((int) bytes[2]) << 8) + ((int) bytes[3]));
}

/*
 * (if p m n)
 * 1. compile p // pushes true / false onto the stack
 * 2. JMP_IF 5 // if p then jump to 5
 * 3. compile n
 * 4. JMP 6
 * 5. compile m
 * 6. NOP
*/
void compile_if(expr *e, op_chunk *target) {
    assert(num_operands(e) == 4);
    assert(e -> value.pair.car -> tag == SYMBOL);
    assert(strcmp(e -> value.pair.car -> value.symbol.s, "if") == 0);

    compile(e -> value.pair.cdr -> value.pair.car, target); // 1

    add_op(target, JMP_IF_OP);
    int jmp_if_instr = target -> size;
    for (int i = 0; i < 4; ++i)
        add_op(target, (uint8_t) 0);

    compile(e -> value.pair.cdr -> value.pair.cdr -> value.pair.cdr -> value.pair.car, target); // 3

    add_op(target, JMP_OP);
    int jmp_instr = target -> size;
    for (int i = 0; i < 4; ++i)
        add_op(target, (uint8_t) 0);

    uint8_t *bytes = to_bytes(target -> size);
    set_op(target, bytes[0], jmp_if_instr);
    set_op(target, bytes[1], jmp_if_instr + 1);
    set_op(target, bytes[2], jmp_if_instr + 2);
    set_op(target, bytes[3], jmp_if_instr + 3);
    
    compile(e -> value.pair.cdr -> value.pair.cdr -> value.pair.car, target); // 5

    uint8_t *bytes2 = to_bytes(target -> size);
    set_op(target, bytes2[0], jmp_instr);
    set_op(target, bytes2[1], jmp_instr + 1);
    set_op(target, bytes2[2], jmp_instr + 2);
    set_op(target, bytes2[3], jmp_instr + 3);

    add_op(target, NO_OP);
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
    case CHAR:
        return load_const(compile_char(e), target);
    case NIL:
        printf("cannot eval the empyt list\n");
        exit(1);
    case PAIR:
        if (e -> value.pair.car -> tag != SYMBOL) {
            printf("not yet implemented higher order functions\n");
            exit(1);
        }
        if (operator_is(e, "quote") && num_operands(e) == 2) {
            return load_const(compile_quoted(e -> value.pair.cdr -> value.pair.car), target);
        } else if (operator_is(e, "if") && num_operands(e) == 4) {
            return compile_if(e, target);
        } else if (operator_is(e, "define") && num_operands(e) == 3) {
        } else if (operator_is(e, "set!") && num_operands(e) == 3) {
        } else if (operator_is(e, "/") && num_operands(e) == 3) {
            compile(e -> value.pair.cdr -> value.pair.car, target);
            compile(e -> value.pair.cdr -> value.pair.cdr -> value.pair.car, target);
            return add_op(target, DIV_OP);
        } else if (operator_is(e, "+")) {
            load_const((stack_object){{0.0}}, target);
            for (int i = 1; i < num_operands(e); ++i) {
                compile(get_at(e, i), target);
                add_op(target, ADD_OP);
            }
            return;
        } else if (operator_is(e, "*")) {
            load_const((stack_object){{1.0}}, target);
            for (int i = 1; i < num_operands(e); ++i) {
                compile(get_at(e, i), target);
                add_op(target, MUL_OP);
            }
            return;
        } else if (operator_is(e, "-")) {
            if (num_operands(e) == 1) {
                fprintf(stderr, "cannot apply - to 0 operands");
            } else if (num_operands(e) == 2) {
                load_const((stack_object){{0.0}}, target);
                compile(e -> value.pair.cdr -> value.pair.car, target);
                add_op(target, SUB_OP);
            } else {
                compile(e -> value.pair.cdr -> value.pair.car, target);
                for (int i = 2; i < num_operands(e); ++i) {
                    compile(get_at(e, i), target);
                    add_op(target, SUB_OP);
                }
            }
        }
    }
}
