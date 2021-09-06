#include <stdio.h>
#include <assert.h>
#include "expr.h"

void test_token_buffer() {
    token_buffer *tb = new_token_buffer();
    for (int i = 0; i < 1000; ++i) {
        extend_token_buffer(tb);
        tb -> buffer[tb -> size - 1].tag = TOKEN_LPAR;
    }
    for (int i = 0; i < 1000; ++i)
        assert(tb -> buffer[i].tag == TOKEN_LPAR);
}

int main() {
    printf("running tests on token buffer\n");
    test_token_buffer();
    printf("tests passed\n");
}
