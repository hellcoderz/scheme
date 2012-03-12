#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "object.h"
#include "mem.h"
#include "reader.h"
#include "log.h"
#include "sstream.h"
#include "sform.h"
#include "gc.h"
#include "objstream.h"

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

static object* parse_number(FILE *in, long sign) {
    int c;
    char is_double = 0;
    object *p;
    char *str_num;
    sstream *stream;

    stream = sstream_new(-1);
    if (stream == NULL) {
        return NULL;
    }

    while (isdigit(c = getc(in))) {
        sstream_append(stream, c);
    }
    if (c == '.' && isdigit(peek(in))) {
        /* flonum */
        is_double = 1;
        sstream_append(stream, c);
        while (isdigit(c = getc(in))) {
            sstream_append(stream, c);
        }
    }
    if (is_delimiter(c)) {
        ungetc(c, in);
        str_num = sstream_cstr(stream);
        if (is_double) {
            double d;
            d = atof(str_num);
            d *= sign;
            p = make_flonum(d);
        } else {
            long n;
            n = atol(str_num);
            n *= sign;
            p = make_fixnum(n);
        }
        sc_free(str_num);
    } else {
        p = NULL;
        fprintf(stderr, 
                "number not followed by delimiter `%c\n", c);
    }

    sstream_dispose(stream);
    return p;
}

static int is_number_start(int c, int ahead) {
    return (isdigit(c) ||
           ((c == '-' || c == '+') && isdigit(ahead)));
}

static int is_boolean_start(int c, int ahead) {
    return c == '#' && ahead != '\\' && ahead != '(';
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
    
    /* continue line 
     * eat whitespace before and after newline */
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
            /* end of string found */
            str = sstream_cstr(stream);
            if (str == NULL) {
                fprintf(stderr, "%s", "no memory\n");
            } else {
                obj = make_string(str);
                sc_free(str);
            }
            break;
        } else if (c == '\\') {
            /* transform escape sequence */
            char x;
            char next;
            
            next = peek(in);
            if (get_escape_char(in, &x) != 0) {
                fprintf(stderr, "unknown escape sequence `%c\n", next);
                break;
            }
            if (x != '\0' && sstream_append(stream, x) != 0) {
                fprintf(stderr, "%s", "internal error\n");
                break;
            }
        } else if (c == EOF) {
            fprintf(stderr, "%s", "EOF encounted\n");
            break;
        } else {
            /* normal charaters */
            if (sstream_append(stream, c) != 0) {
                fprintf(stderr, "%s", "internal error\n");
                break;
            }
        }
    }

    sstream_dispose(stream);
    return obj;
}

static int is_list_start(int c) {
    return c == '(';
}

static object* parse_list(FILE *in) {
    int c;
    object *car_obj, *cdr_obj;
    object *list;

    skip_whitespace(in);
    c = peek(in);
    if (c == ')') {
        getc(in);
        return get_empty_list();
    }

    car_obj = sc_read(in);
    if (car_obj == NULL) {
        return NULL;
    }

    skip_whitespace(in);
    c = peek(in);
    if (c == '.') {
        /* improper list */
        getc(in);
        c = peek(in);
        if (!is_delimiter(c)) {
            fprintf(stderr, "%s `%c\n",
                    "dot not followed by delimiter", c);
            return NULL;
        }

        gc_protect(car_obj);
        cdr_obj = sc_read(in);
        gc_abandon();
        if (cdr_obj == NULL) {
            return NULL;
        }
        skip_whitespace(in);
        c = getc(in);
        if (c != ')') {
            fprintf(stderr, "%s `%c\n",
                    "missing close parentheses", c);
            return NULL;
        }

        gc_protect(car_obj);
        gc_protect(cdr_obj);
        list = cons(car_obj, cdr_obj);
        gc_abandon();
        gc_abandon();
        return list;
    } else {
        /* proper list */
        gc_protect(car_obj);
        cdr_obj = parse_list(in);
        gc_abandon();

        if (cdr_obj == NULL) {
            return NULL;
        }

        gc_protect(car_obj);
        gc_protect(cdr_obj);
        list = cons(car_obj, cdr_obj);
        gc_abandon();
        gc_abandon();
        return list;
    }

    /* should never get here */
    return NULL;
}

static int is_symbol_start(int c, int ahead) {
    if (isalpha(c) ||
        ((c == '+' || c == '-') && !isdigit(ahead)) ||
        c == '!' || c == '$' || c == '%' || c == '&' ||
        c == '*' || c == '.' || c == '/' || c == ':' ||
        c == '<' || c == '=' || c == '>' || c == '?' ||
        c == '@' || c == '^' || c == '_' || c == '~') {
        return 1;
    }
    return 0;
}

static int is_symbol_char(int c) {
    if (isalpha(c) || isdigit(c) || c == '+' || c == '-' ||
        c == '!' || c == '$' || c == '%' || c == '&' ||
        c == '*' || c == '.' || c == '/' || c == ':' ||
        c == '<' || c == '=' || c == '>' || c == '?' ||
        c == '@' || c == '^' || c == '_' || c == '~') {
        return 1;
    }
    return 0;
}

static object* parse_symbol(FILE *in) {
    object *obj = NULL;
    int c;
    sstream *stream;

    stream = sstream_new(-1);
    if (stream == NULL) {
        fprintf(stderr, "%s", "no memory for stream\n");
        return NULL;
    }
    
    for (;;) {
        c = getc(in);
        if (is_symbol_char(c)) {
            int ret;
            ret = sstream_append(stream, c);
            if (ret != 0) {
                fprintf(stderr, "%s", "internal error\n");
                break;
            }
        } else {
            if (is_delimiter(c)) {
                char *sym;
                ungetc(c, in);
                sym = sstream_cstr(stream);
                if (sym == NULL) {
                    fprintf(stderr, "%s", "internal error\n");
                } else {
                    obj = make_symbol(sym);
                    sc_free(sym);
                }
            } else {
                fprintf(stderr, "%s `%c\n",
                        "unexpected input", c);
            }
            break;
        }
    }

    sstream_dispose(stream);
    return obj;
}

static int is_quote_start(int c) {
    return c == '\'';
}

static object* parse_quote_form(FILE *in) {
    object *quote, *obj, *cdr_obj;

    quote = get_quote_symbol();
    cdr_obj = sc_read(in);
    if (cdr_obj == NULL) {
        obj = NULL;
    } else {
        gc_protect(cdr_obj);
        obj = cons(quote, cons(cdr_obj, get_empty_list()));
        gc_abandon();
    }

    return obj;
}

int is_vector_start(int c, int ahead) {
    return c == '#' && ahead == '(';
}

object* parse_vector(FILE *in) {
    objstream *stream;
    object *obj;
    object **buf;
    int ret;

    stream = objstream_new(-1);
    if (stream == NULL) {
        fprintf(stdout, "null");
        return NULL;
    }

    for(;;) {
        skip_whitespace(in);
        if (peek(in) == ')') {
            getc(in);
            break;
        }
        obj = sc_read(in);
        if (obj == NULL) {
            objstream_dispose(stream);
            return NULL;
        }
        ret = objstream_append(stream, obj);
        if (ret != 0) {
            objstream_dispose(stream);
            return NULL;
        }
    }

    if (!is_delimiter(peek(in))) {
        fprintf(stdout, "vector not followed by delimiter `%c\n", peek(in));
        objstream_dispose(stream);
        return NULL;
    }

    buf = objstream_trim(stream, &ret);
    gc();
    obj = make_vector(buf, ret);
    objstream_dispose(stream);
    return obj;
}

object* sc_read(FILE *in) {
    int c;
    object *obj;

    skip_whitespace(in);

    c = getc(in);
    if (is_number_start(c, peek(in))) {
        long sign = 1;

        if (c == '-') {
            sign = -1;
        }
        if (isdigit(c)) {
            ungetc(c, in);
        }
        obj = parse_number(in, sign);
    } else if (is_boolean_start(c, peek(in))) {
        obj = parse_boolean(in);
    } else if (is_character_start(c, peek(in))) {
        getc(in);
        obj = parse_character(in);
    } else if (is_string_start(c)) {
        obj = parse_string(in);
    } else if (is_list_start(c)) {
        obj = parse_list(in);
    } else if (is_vector_start(c, peek(in))) {
        getc(in);
        obj = parse_vector(in);
    } else if (is_symbol_start(c, peek(in))) {
        ungetc(c, in);
        obj = parse_symbol(in);
    } else if (is_quote_start(c)) {
        obj = parse_quote_form(in);
    } else if (c == EOF) {
        obj = get_eof_object();
    } else {
        fprintf(stderr, "bad input, at `%c\n", c);
        obj = NULL;
    }

    return obj;
}

