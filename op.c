#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "op.h"
#include "compiler.h"

const_array *new_const_array() {
    const_array *result = malloc(sizeof(const_array));
    result -> size = 0;
    result -> capacity = 1;
    result -> constants = malloc(sizeof(stack_object));
    return result;
}

void add_const(const_array *c, stack_object o) {
    if (c -> size == c -> capacity) {
        c -> capacity *= 2;
        stack_object *new_data = malloc(sizeof(stack_object) * c -> capacity);
        for (int i = 0; i < c -> size; ++i)
            new_data[i] = c -> constants[i];
        free(c -> constants);
        c -> constants = new_data;
    }
    c -> constants[c -> size] = o;
    ++ c -> size;
}


op_chunk *new_op_chunk() {
    op_chunk *result = malloc(sizeof(op_chunk));
    result -> capacity = 1;
    result -> size = 0;
    result -> data = malloc(sizeof(uint8_t));
    result -> constants = new_const_array();
    return result;
}

void add_op(op_chunk *c, uint8_t op) {
    if (c -> size == c -> capacity) {
        c -> capacity *= 2;
        uint8_t *new_data = malloc(sizeof(uint8_t) * c -> capacity);
        for (int i = 0; i < c -> size; ++i)
            new_data[i] = c -> data[i];
        free(c -> data);
        c -> data = new_data;
    }
    c -> data[c -> size] = op;
    ++ c -> size;

}

void set_op(op_chunk *c, uint8_t op, int index) {
    assert(index < c -> size);
    c -> data[index] = op;

}

union so_bytes {
    stack_object so;
    uint64_t bytes;
};

void print_stack_object(stack_object *so) {
    union so_bytes bytes;
    bytes.so = *so;
    for (int i = sizeof(uint64_t) * 8 - 1; i >= 0; --i) {
        if ((bytes.bytes >> i) & 1u)
            printf("1");
        else
            printf("0");
    }
    // printf("%f", so -> number.d);
}

int disassembleOp(op_chunk *c, int i) {
    printf("%04x ", i);

    switch (c -> data[i]) {
    case RETURN_OP:
        printf("RET\n");
        return i + 1;
    case LOAD_CONST_OP:
        printf("LOAD_CONST ");
        print_stack_object(c -> constants -> constants + c -> data[i + 1]);
        printf("\n");
        return i + 2;
    case ADD_OP:
        printf("ADD\n");
        return i + 1;
    case MUL_OP:
        printf("MUL\n");
        return i + 1;
    case DIV_OP:
        printf("DIV\n");
        return i + 1;
    case SUB_OP:
        printf("SUB\n");
        return i + 1;
    case LOAD_TRUE_OP:
        printf("LOAD_TRUE\n");
        return i + 1;
    case LOAD_FALSE_OP:
        printf("LOAD_FALSE\n");
        return i + 1;
    case JMP_OP:
        {
            uint64_t target = c -> data[i + 1] << 24;
            target += ((uint64_t) c -> data[i + 2]) << 16;
            target += ((uint64_t) c -> data[i + 3]) << 8;
            target += ((uint64_t) c -> data[i + 4]);
            printf("JMP %d\n", (int) target);
        }
        return i + 5;
    case JMP_IF_OP:
        {
            uint64_t target = c -> data[i + 1] << 24;
            target += ((uint64_t) c -> data[i + 2]) << 16;
            target += ((uint64_t) c -> data[i + 3]) << 8;
            target += ((uint64_t) c -> data[i + 4]);
            printf("JMP IF %d\n", (int) target);
        }
        return i + 5;
    case POP_OP:
        printf("POP\n");
        return i + 1;
    case NO_OP:
        printf("NO_OP\n");
        return i + 1;
    default:
        fprintf(stderr, "unknown instruction %d\n", c -> data[i]);
        exit(1);
    }
}

void disassembleOpChunk(op_chunk *c) {
    printf("=== op chunk ===\n");
    for (int i = 0; i < c -> size;)
        i = disassembleOp(c, i);
    printf("================\n");
}
