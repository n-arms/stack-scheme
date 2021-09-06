#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "expr.h"
#include "lexer.h"
#include "parser.h"
#include "op.h"
#include "check.h"
#include "compiler.h"

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

void test_expr_validation() {
    expr *e;
    
    e = parse_expr(scan("#t"));
    assert(validate(e));

    e = parse_expr(scan("123"));
    assert(validate(e));

    e = parse_expr(scan("\"abc\""));
    assert(validate(e));

    e = parse_expr(scan("(if #t 1 2)"));
    assert(validate(e));

    e = parse_expr(scan("(define a 4)"));
    assert(validate(e));

    e = parse_expr(scan("(set! a 5)"));
    assert(validate(e));

    e = parse_expr(scan("'5"));
    assert(validate(e));

    e = parse_expr(scan("(quote 5)"));
    assert(validate(e));
}

void test_atomic_compiler() {
    op_chunk *o;

    o = compile(parse_expr(scan("42")), 32);
    assert(o -> size == 1);
    assert(o -> data[0] == LOAD_CONST_OP);
    assert(o -> constants[0].number.d == 42);

    o = compile(parse_expr(scan("#t")), 32);
    assert(o -> size == 1);
    assert(o -> data[0] == LOAD_CONST_OP);
    assert(o -> constants[0].boolean.b == true);

    o = compile(parse_expr(scan("abc")), 32);
    assert(o -> size == 1);
    assert(o -> data[0] == LOAD_CONST_OP);
    assert(o -> constants[0].heap_ref.ref -> object_type == SYMBOL_OBJ);
    assert(strcmp(o -> constants[0].heap_ref.ref -> value.symbol.s, "abc") == 0);

    o = compile(parse_expr(scan("\"do re mi\"")), 32);
    assert(o -> size == 1);
    assert(o -> data[0] == LOAD_CONST_OP);
    assert(o -> constants[0].heap_ref.ref -> object_type == STRING_OBJ);
    assert(strcmp(o -> constants[0].heap_ref.ref -> value.string.s, "do re mi") == 0);
}

void test() {
    atexit(cleanup);

    TEST(test_token_buffer);

    TEST(test_single_token);

    TEST(test_full_scan);

    TEST(test_parse_atom);

    TEST(test_parse);

    TEST(test_op_chunk);

    TEST(test_expr_validation);
}

int main() {
    test();
}
