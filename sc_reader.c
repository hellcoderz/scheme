#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "sc_object.h"
#include "sc_mem.h"
#include "sc_reader.h"
#include "sc_log.h"

static int is_delimiter(int c) {
    return isspace(c) || c == EOF ||
           c == '(' || c == ')' ||
           c == '"' || c == ';';
}

static int peek(FILE *in) {
    int c;

    c = getc(in);
    ungetc(c, in);
    return c;
}

static void skip_whitespace(FILE *in) {
    int c;

    while ((c = getc(in)) != EOF) {
        if (isspace(c)) {
            continue;
        } else if (c == ';') {
            /* Skip line comment */
            while (((c = getc(in)) != EOF) && (c != '\n'));
            continue;
        }

        ungetc(c, in);
        break;
    }
}

static object* parse_fixnum(FILE *in, long sign) {
    int c;
    long num = 0;
    object *p;

    while (isdigit(c = getc(in))) {
        num = (num * 10) + (c - '0');
    }
    num *= sign;

    ungetc(c, in);
    if (is_delimiter(c)) {
        p = make_fixnum(num);
    } else {
        p = NULL;
        fprintf(stderr, 
                "fixnum not followed by delimiter(%c)\n", c);
    }
    return p;
}

static int is_fixnum_start(int c, int ahead) {
    return (isdigit(c) ||
           ((c == '-' || c == '+') && isdigit(ahead)));
}

object* sc_read(FILE *in) {
    int c;
    object *obj;

    skip_whitespace(in);

    c = getc(in);
    if (is_fixnum_start(c, peek(in))) {
        long sign = 1;

        if (c == '-') {
            sign = -1;
        }
        if (isdigit(c)) {
            ungetc(c, in);
        }
        obj = parse_fixnum(in, sign);
    } else {
        fprintf(stderr, "bad input, unexpected `%c\n", c);
        obj = NULL;
    }
    return obj;
}

