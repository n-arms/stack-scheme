#include "expr.h"
#include <stdlib.h>

token_buffer *new_token_buffer() {
    token_buffer *tb = malloc(sizeof(token_buffer));
    tb -> index = 0;
    tb -> size = 0;
    tb -> capacity = 1;
    tb -> buffer = malloc(sizeof(token));
    return tb;
}

void extend_token_buffer(token_buffer *tb) {
    if (tb -> capacity == tb -> size) {
        token *new_buffer = malloc(sizeof(token) * tb -> capacity * 2);
        for (int i = 0; i < tb -> size; ++i)
            new_buffer[i] = tb -> buffer[i];
        tb -> buffer = new_buffer;
        tb -> capacity *= 2;
    }
    ++ tb -> size;
}
