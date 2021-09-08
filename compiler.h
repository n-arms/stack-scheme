#ifndef COMPILER_H_
#define COMPILER_H_

#include "op.h"
#include "expr.h"

// after running "compile" on the vm, there should be 1 object on the stack
void compile(expr *e, op_chunk *target);

uint8_t *to_bytes(int i);
int from_bytes(uint8_t *bytes);

#endif
