#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "lexer.h"

#define STRING_BUFFER 32

token_buffer *scan(char *text) {
    int counter = 0;
    token_buffer *tb = new_token_buffer();
loop:
    switch (text[counter]) {
    case '(':
        extend_token_buffer(tb);
        tb -> buffer[tb -> size - 1].tag = TOKEN_LPAR;
        ++counter;
        goto loop;
    case ')':
        extend_token_buffer(tb);
        tb -> buffer[tb -> size - 1].tag = TOKEN_RPAR;
        ++counter;
        goto loop;
    case '.':
        extend_token_buffer(tb);
        tb -> buffer[tb -> size - 1].tag = TOKEN_DOT;
        ++counter;
        goto loop;
    case '\'':
        extend_token_buffer(tb);
        tb -> buffer[tb -> size - 1].tag = TOKEN_QUOTE;
        ++counter;
        goto loop;
    case ' ':
        ++counter;
        goto loop;
    case '\n':
        ++counter;
        goto loop;
    case '#':
        ++counter;
        switch (text[counter]) {
            case 't':
                extend_token_buffer(tb);
                tb -> buffer[tb -> size - 1].tag = TOKEN_TRUE;
                ++counter;
                goto loop;
            case 'f':
                extend_token_buffer(tb);
                tb -> buffer[tb -> size - 1].tag = TOKEN_FALSE;
                ++counter;
                goto loop;
            default:
                fprintf(stderr, "neither t nor f was found after #\n");
                return NULL;
        }
    case '"':
        ++counter;
        int i;
        char *buffer = malloc(sizeof(char) * (STRING_BUFFER + 1));
        for (i = 0; text[counter + i] != '"'; ++i) {
            buffer[i] = text[counter + i];
        }
        counter += i + 1;
        buffer[i] = '\0';
        extend_token_buffer(tb);
        tb -> buffer[tb -> size - 1].tag = TOKEN_STRING;
        tb -> buffer[tb -> size - 1].value.string.s = buffer;
        goto loop;
    case '\0':
        tb -> index = 0;
        return tb;
    default:
        if ((text[counter] == '-' && isdigit(text[counter + 1])) || isdigit(text[counter])) {
            int i;
            char *buffer = malloc(sizeof(char) * (STRING_BUFFER + 1));
            for (i = 0; (
                        isdigit(text[counter + i]) || 
                        text[counter + i] == '.' || 
                        text[counter + i] == '-') && 
                        text[counter + i] != '\0'; ++i) {
                buffer[i] = text[counter + i];
            }
            counter += i;
            buffer[i] = '\0';
            extend_token_buffer(tb);
            tb -> buffer[tb -> size - 1].tag = TOKEN_NUMBER;
            tb -> buffer[tb -> size - 1].value.number.d = strtod(buffer, NULL);
            goto loop;
        } else {
            int i;
            char *buffer = malloc(sizeof(char) * (STRING_BUFFER + 1));
            for (i = 0; 
                    text[counter + i] != ' ' && 
                    text[counter + i] != '\n' && 
                    text[counter + i] != '\0' &&
                    text[counter + i] != '(' &&
                    text[counter + i] != ')'; ++i) {
                buffer[i] = text[counter + i];
            }
            counter += i;
            buffer[i] = '\0';
            extend_token_buffer(tb);
            tb -> buffer[tb -> size - 1].tag = TOKEN_SYMBOL;
            tb -> buffer[tb -> size - 1].value.symbol.s = buffer;
            goto loop;
        }
    }

    fprintf(stderr, "lexer.scan exited abnormally\n");
    return NULL;
}
