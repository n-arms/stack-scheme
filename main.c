#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "expr.h"
#include "lexer.h"
#include "parser.h"
#include "op.h"

#define TEST(f) \
    fprintf(stderr, "\nrunning " #f "\n\e[0;31m"); \
    f(); \
    fprintf(stderr, "\e[0mtest passed\n");

void cleanup() {
    fprintf(stderr, "\e[0m\n");
}

void test_token_buffer() {
    token_buffer *tb = new_token_buffer();
    for (int i = 0; i < 1000; ++i) {
        extend_token_buffer(tb);
        tb -> buffer[tb -> size - 1].tag = TOKEN_LPAR;
    }
    for (int i = 0; i < 1000; ++i)
        assert(tb -> buffer[i].tag == TOKEN_LPAR);
}

void test_single_token() {
    token_buffer *tb;
    
    tb = scan("\"abc\"");
    assert(tb -> buffer[0].tag == TOKEN_STRING);

    tb = scan("abc");
    assert(tb -> buffer[0].tag == TOKEN_SYMBOL);

    tb = scan("+");
    assert(tb -> buffer[0].tag == TOKEN_SYMBOL);

    tb = scan("'");
    assert(tb -> buffer[0].tag == TOKEN_QUOTE);

    tb = scan("(");
    assert(tb -> buffer[0].tag == TOKEN_LPAR);

    tb = scan(")");
    assert(tb -> buffer[0].tag == TOKEN_RPAR);

    tb = scan("#t");
    assert(tb -> buffer[0].tag == TOKEN_TRUE);

    tb = scan("#f");
    assert(tb -> buffer[0].tag == TOKEN_FALSE);

    tb = scan("123");
    assert(tb -> buffer[0].tag == TOKEN_NUMBER);
    assert(tb -> buffer[0].value.number.d == 123.0);

    tb = scan("-123");
    assert(tb -> buffer[0].tag == TOKEN_NUMBER);
    assert(tb -> buffer[0].value.number.d == -123.0);

    tb = scan("0.25");
    assert(tb -> buffer[0].tag == TOKEN_NUMBER);
    assert(tb -> buffer[0].value.number.d == 0.25);

    tb = scan("-0.25");
    assert(tb -> buffer[0].tag == TOKEN_NUMBER);
    assert(tb -> buffer[0].value.number.d == -0.25);
}

void test_full_scan() {
    token_buffer *tb;

    tb = scan("(abc #t (\"123\" (0.25 #f ())))");
}

void test_parse() {
    token_buffer *tb;
    expr *e;

    tb = scan("( 123 abc #t () \"one two three\" )");
    e = parse_expr(tb);
    print_expr(e);
    printf("\n");
}

void test_parse_atom() {
    token_buffer *tb;
    expr *e;

    tb = scan("123");
    e = parse_expr(tb);
    assert(e -> tag == NUMBER);
    assert(e -> value.number.d == 123);

    tb = scan("abc");
    e = parse_expr(tb);
    assert(e -> tag == SYMBOL);
    assert(!strcmp(e -> value.symbol.s, "abc"));

    tb = scan("\"abc\"");
    e = parse_expr(tb);
    assert(e -> tag == STRING);
    assert(!strcmp(e -> value.string.s, "abc"));

    tb = scan("#t");
    e = parse_expr(tb);
    assert(e -> tag == BOOLEAN);
    assert(e -> value.boolean.b);
    
    tb = scan("#f");
    e = parse_expr(tb);
    assert(e -> tag == BOOLEAN);
    assert(!e -> value.boolean.b);
}

void test_op_chunk() {
    op_chunk *o = new_op_chunk(0);
    for (int i = 0; i < 1000; ++i)
        add_op(o, RETURN_OP);
    for (int i = 0; i < 1000; ++i)
        assert(o -> data[i] == RETURN_OP);
}

void test() {
    atexit(cleanup);

    TEST(test_token_buffer);

    TEST(test_single_token);

    TEST(test_full_scan);

    TEST(test_parse_atom);

    TEST(test_parse);

    TEST(test_op_chunk);
}

int main() {
    test();

    op_chunk *o = new_op_chunk(32);
    o -> constants -> boolean.b = 0;
    o -> constants[1].boolean.b = 1;

    add_op(o, LOAD_CONST_OP);
    add_op(o, 0);
    add_op(o, LOAD_CONST_OP);
    add_op(o, 1);
    add_op(o, ADD_OP);
    add_op(o, RETURN_OP);

    disassembleOpChunk(o);
}
