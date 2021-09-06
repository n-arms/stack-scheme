#ifndef OP_H_
#define OP_H_

#include <stdint.h>
#include <stdbool.h>

#define RETURN_OP ((uint8_t) 0)
#define LOAD_CONST_OP ((uint8_t) 1)
#define ADD_OP ((uint8_t) 2)
#define SUB_OP ((uint8_t) 3)
#define DIV_OP ((uint8_t) 4)
#define MUL_OP ((uint8_t) 5)

union stack_object;

// the list of instructions
typedef struct {
    int capacity;
    int size;
    uint8_t *data;
    union stack_object *constants;
} op_chunk;

op_chunk *new_op_chunk(int const_max);
void add_op(op_chunk *c, uint8_t op);

int disassembleOp(op_chunk* c, int i);
void disassembleOpChunk(op_chunk *c);

struct heap_object;

// living on the stack we have 64 bit objects:
typedef union stack_object {
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
