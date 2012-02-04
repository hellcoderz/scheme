#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "sc_object.h"
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
                "fixnum not followed by delimiter `%c\n", c);
    }
    return p;
}

static int is_fixnum_start(int c, int ahead) {
    return (isdigit(c) ||
           ((c == '-' || c == '+') && isdigit(ahead)));
}

static int is_boolean_start(int c, int ahead) {
    return c == '#' && (ahead == 't' || ahead == 'f'); 
}

static int is_character_start(int c, int ahead) {
    return c == '#' && ahead == '\\';
}

static object* try_character(FILE *in, char *name, char val) {
    char *s = name + 1;

    while (*s != '\0' && peek(in) == *s) {
        s++;
        getc(in);
    }
    if (*s == '\0' && is_delimiter(peek(in))) {
        return make_character(val);
    }

    return NULL;
}

static object* parse_character(FILE *in) {
    int c, ahead;
    object *obj = NULL;

    c = getc(in);
    ahead = peek(in);
    switch (c) {
        case EOF:
            fprintf(stderr, "%s", "incomplete character literal\n");
            break;
        case LINEFEED:
        case TAB:
        case SPACE:
        case RETURN:
            fprintf(stderr, "%s", "spaces are not allowed in character literal\n");
            break;

        case 'n':
            if (ahead == 'u') {
                obj = try_character(in, "nul", NUL);
            } else if (ahead == 'e') {
                obj = try_character(in, "newline", NEWLINE);
            }
            break;

        case 't':
            if (ahead == 'a') {
                obj = try_character(in, "tab", TAB);
            }
            break;

        case 'l':
            if (ahead == 'i') {
                obj = try_character(in, "linefeed", LINEFEED);
            }
            break;

        case 'r':
            if (ahead == 'e') {
                obj = try_character(in, "return", RETURN);
            }
            break;

        case 's':
            if (ahead == 'p') {
                obj = try_character(in, "space", SPACE);
            }
            break;
    }

    if (obj == NULL && is_delimiter(ahead) && !isspace(c)) {
        obj = make_character(c);
    }

    if (obj == NULL && !isspace(c)) {
        fprintf(stderr, "bad character literal `%c\n", ahead);
    }

    return obj;
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
    } else if (is_boolean_start(c, peek(in))) {
        int v;

        v = getc(in);
        obj = make_boolean(v);
    } else if(is_character_start(c, peek(in))) {
        getc(in);
        obj = parse_character(in);
    } else {
        fprintf(stderr, "bad input, at `%c\n", c);
        obj = NULL;
    }

    return obj;
}

