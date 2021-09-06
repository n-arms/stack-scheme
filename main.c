#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "expr.h"
#include "lexer.h"
#include "parser.h"

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

int main() {
    atexit(cleanup);

    TEST(test_token_buffer);

    TEST(test_single_token);

    TEST(test_full_scan);

    TEST(test_parse_atom);
}
