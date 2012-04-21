#include <stdio.h>
#include <ctype.h>
#include "object.h"
#include "write.h"
#include "log.h"
#include "sform.h"

static int write_character(FILE *out, object *val) {
    char v = obj_cv(val);
    
    fprintf(out, "%s", "#\\");
    switch (v) {
        case NUL:
            fprintf(out, "%s", "nul");
            break;
        case TAB:
            fprintf(out, "%s", "tab");
            break;
        /* LINEFEED is equal to NEWLINE */
        case NEWLINE:
            fprintf(out, "%s", "newline");
            break;
        case RETURN:
            fprintf(out, "%s", "return");
            break;
        case SPACE:
            fprintf(out, "%s", "space");
            break;
        default:
            fprintf(out, "%c", v);
            if (!isgraph(v)) {
                sc_log("character not printable `%d", v);
            }
            break;
    }
    return 0;
}

static int write_string(FILE *out, object *val) {
    char *str = obj_sv(val);

    fputc('"', out);
    while (*str != '\0') {
       switch (*str) {
           case '\a':
               fprintf(out, "%s", "\\a");
               break;
           case '\b':
               fprintf(out, "%s", "\\b");
               break;
           case '\t':
               fprintf(out, "%s", "\\t");
               break;
           case '\n':
               fprintf(out, "%s", "\\n");
               break;
           case '\v':
               fprintf(out, "%s", "\\v");
               break;
           case '\f':
               fprintf(out, "%s", "\\f");
               break;
           case '\r':
               fprintf(out, "%s", "\\r");
               break;
           case '"':
               fprintf(out, "%s", "\\\"");
               break;
           case '\\':
               fprintf(out, "%s", "\\\\");
               break;
           default:
               fputc(*str, out);
               break;
       }
       str++;
    }
    fputc('"', out);

    return 0;
}

static int write_pair(FILE *out, object *val) {
    object *car_obj, *cdr_obj;

    car_obj = car(val);
    cdr_obj = cdr(val);

    sc_write(out, car_obj);
    if (is_pair(cdr_obj)) {
        fprintf(out, " ");
        write_pair(out, cdr_obj);
    } else if (is_empty_list(cdr_obj)) {
        return 0;
    } else {
        fprintf(out, " . ");
        sc_write(out, cdr_obj);
    }
    return 0;
}

int write_vector(FILE *out, object *obj) {
    int i, len, ret;
    object **buf;

    len = obj_vsv(obj);
    buf = obj_vav(obj);
    for (i = 0; i < len - 1; i++) {
        ret = sc_write(out, buf[i]);
        if (ret != 0) {
            return ret;
        }
        fprintf(out, " ");
    }
    if (len > 0) {
        sc_write(out, buf[len-1]);
    }
    return 0;
}

static int is_quote(object *val) {
    return car(val) == get_quote_symbol();
}

static int is_quasiquote(object *val) {
    return car(val) == get_quasiquote_symbol();
}

static int is_unquote(object *val) {
    return car(val) == get_unquote_symbol();
}

static int is_unquotesplicing(object *val) {
    return car(val) == get_unquotesplicing_symbol();
}

static object* handle_quote(FILE *out, object *val) {
    object *ret = val;
    /* successive quotes needs a loop to remove them all */
    for (;;) {
        if (is_quote(ret)) {
            ret = cadr(ret);
            fprintf(out, "'");
        } else if (is_quasiquote(ret)) {
            ret = cadr(ret);
            fprintf(out, "`");
        } else if (is_unquote(ret)) {
            ret = cadr(ret);
            fprintf(out, ",");
        } else if (is_unquotesplicing(ret)) {
            ret = cadr(ret);
            fprintf(out, ",@");
        } else {
            break;
        }
    }
    return ret;
}

int sc_write(FILE *out, object *val) {
    int ret = 0;

    if (val == NULL) {
        sc_log("%s", "Cannot print NULL val");
        return -1;
    }

    if (is_fixnum(val)) {
        fprintf(out, "%ld", obj_nv(val));
    } else if (is_flonum(val)) {
        fprintf(out, "%.16g", obj_rv(val));
    } else if (is_boolean(val)) {
        char v = is_true(val) ? 't' : 'f';
        fprintf(out, "#%c", v);
    } else if (is_character(val)) {
        ret = write_character(out, val);
    } else if (is_string(val)) {
        ret = write_string(out, val);
    } else if (is_empty_list(val)) {
        fprintf(out, "%s", "()");
    } else if (is_pair(val)) {
        val = handle_quote(out, val);
        if (is_pair(val)) {
            fprintf(out, "(");
            ret = write_pair(out, val);
            fprintf(out, ")");
        } else {
            sc_write(out, val);
        }
    } else if (is_vector(val)) {
        fprintf(out, "#(");
        ret = write_vector(out, val);
        fprintf(out, ")");
    } else if (is_symbol(val)) {
        fprintf(out, "%s", obj_iv(val));
    } else if (is_compound_proc(val) || is_primitive_proc(val)) {
        fprintf(out, "#<procedure@%p>", val);
    } else if (is_eof_object(val)) {
        fprintf(out, "%s", "#<eof>");
    } else if (is_input_port(val)) {
        fprintf(out, "#<input-port@%p>", val);
    } else if (is_output_port(val)) {
        fprintf(out, "#<output-port@%p>", val);
    } else if (is_env_frame(val)) {
        fprintf(out, "#<environment-frame@%p>", val);
    } else if (is_cont(val)) {
        fprintf(out, "#<continuation@%p>", val);
    } else if (is_macro(val)) {
        fprintf(out, "#<macro@%p>", val);
    } else {
        fprintf(stderr,
                "unknown type, cannot print\n");
        ret = 1;
    }

    return ret;
}

