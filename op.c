#include <stdio.h>
#include <stdlib.h>
#include "op.h"

op_chunk *new_op_chunk(int const_max) {
    op_chunk *result = malloc(sizeof(op_chunk));
    result -> capacity = 1;
    result -> size = 0;
    result -> data = malloc(sizeof(uint8_t));
    result -> constants = malloc(sizeof(stack_object) * const_max);
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
}

int disassembleOp(op_chunk *c, int i) {
    printf("%04x ", i);

    switch (c -> data[i]) {
    case RETURN_OP:
        printf("RET\n");
        return i + 1;
    case LOAD_CONST_OP:
        printf("LOAD_CONST ");
        print_stack_object(c -> constants + c -> data[i + 1]);
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
