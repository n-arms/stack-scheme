#ifndef COMPILER_H_
#define COMPILER_H_

#include "op.h"
#include "expr.h"

void compile(expr *e, op_chunk *target);

#endif
