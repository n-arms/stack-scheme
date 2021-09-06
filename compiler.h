#ifndef COMPILER_H_
#define COMPILER_H_

#include "op.h"
#include "expr.h"

op_chunk *compile(expr *e, int const_max);

#endif
