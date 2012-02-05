#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "sc_object.h"
#include "sc_reader.h"
#include "sc_log.h"
#include "sc_sstream.h"

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
    return c == '#' && ahead != '\\';
}

static object* parse_boolean(FILE *in) {
    int c, next;
    object *obj = NULL;

    next = getc(in);
    c = peek(in);
    if ((next == 't' || next == 'f') && is_delimiter(c)) {
        obj = make_boolean(next);
    } else {
        fprintf(stderr, "unexpected `%c\n", next);
    }
    
    return obj;
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

static object* try_parse_character(int c, int ahead, FILE *in) {
    object *obj = NULL;

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
    return obj;
}

static object* parse_character(FILE *in) {
    int c, ahead;
    object *obj; 

    c = getc(in);
    ahead = peek(in);
    obj = try_parse_character(c, ahead, in);

    if (obj == NULL && is_delimiter(ahead) && !isspace(c)) {
        obj = make_character(c);
    }

    if (obj == NULL && !isspace(c)) {
        fprintf(stderr, "bad character literal `%c\n", ahead);
    }

    return obj;
}

static int is_string_start(int c) {
    return c == '"';
}

static int get_escape_char(FILE *in, char *p) {
    char c;

    c = getc(in);
    switch (c) {
        case 'a':
            *p = '\a';
            return 0;
        case 'b':
            *p = '\b';
            return 0;
        case 't':
            *p = '\t';
            return 0;
        case 'n':
            *p = '\n';
            return 0;
        case 'v':
            *p = '\v';
            return 0;
        case 'f':
            *p = '\f';
            return 0;
        case 'r':
            *p = '\r';
            return 0;
        case '"':
            *p = '"';
            return 0;
        case '\\':
            *p = '\\';
            return 0;
    }
    
    /* continue line */
    if (isspace(c)) {
        while (c != '\n') {
            c = getc(in);
            if (!isspace(c)) {
                break;
            }
        }
        if (c == '\n') {
            while (isspace(c = peek(in)) && c != '\n') {
                getc(in);
            }
            *p = '\0';
        } else {
            return -1;
        }
    } else {
        return -1;
    }

    return 0;
}  

static object* parse_string(FILE *in) {
    object *obj = NULL;
    sstream *stream;
    char *str;
    char c;

    stream = sstream_new(-1);
    if (stream == NULL) {
        fprintf(stderr, "%s", "no memory\n");
        return NULL;
    }
    
    for (;;) {
        c = getc(in);
        if (c == '"') {
            str = sstream_cstr(stream);
            if (str == NULL) {
                fprintf(stderr, "%s", "no memory\n");
                return NULL;
            }
            sstream_dispose(stream);
            obj = make_string(str);
            break;
        } else if (c == '\\') {
            char x;
            char next;
            
            next = peek(in);
            if (get_escape_char(in, &x) != 0) {
                fprintf(stderr, "unknown escape sequence `%c\n", next);
                return NULL;
            }
            if (x != '\0' && sstream_append(stream, x) != 0) {
                fprintf(stderr, "%s", "no memory\n");
                return NULL;
            }
        } else if (c == EOF) {
            fprintf(stderr, "%s", "EOF encounted\n");
            return NULL;
        } else {
            if (sstream_append(stream, c) != 0) {
                fprintf(stderr, "%s", "no memory\n");
                return NULL;
            }
        }
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
        obj = parse_boolean(in);
    } else if (is_character_start(c, peek(in))) {
        getc(in);
        obj = parse_character(in);
    } else if (is_string_start(c)) {
        obj = parse_string(in);
    } else {
        fprintf(stderr, "bad input, at `%c\n", c);
        obj = NULL;
    }

    return obj;
}

