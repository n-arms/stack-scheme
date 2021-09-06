#include <stdio.h>
#include <stdlib.h>
#include "op.h"

op_chunk *new_op_chunk() {
    op_chunk *result = malloc(sizeof(op_chunk));
    result -> capacity = 1;
    result -> size = 0;
    result -> data = malloc(sizeof(uint8_t));
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

int disassembleOp(op_chunk *c, int i) {
    printf("%04x ", i);

    switch (c -> data[i]) {
    case RETURN_OP:
        printf("RET\n");
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
