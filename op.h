#ifndef OP_H_
#define OP_H_

#include <stdint.h>
#include <stdbool.h>

#define RETURN_OP ((char) 0)

// the list of instructions
typedef struct {
    int capacity;
    int size;
    uint8_t *data;
} op_chunk;

op_chunk *new_op_chunk();
void add_op(op_chunk *c, uint8_t op);

int disassembleOp(op_chunk* c, int i);
void disassembleOpChunk(op_chunk *c);

struct heap_object;

// living on the stack we have 64 bit objects:
typedef union {
    struct {
        double d;
    } number;
    struct {
        bool b;
    } boolean;
    struct {
        struct heap_object *ref;
    } heap_ref;
} stack_object;

// and living on the heap we have 72 bit objects:
typedef struct heap_object {
    union {
        struct {
            char *s;
        } string;
        struct {
            char *s;
        } symbol;
    } value;
    char object_type;
} heap_object;

#endif
