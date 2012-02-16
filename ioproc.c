#include <stdio.h>
#include <stdlib.h>
#include "object.h"
#include "ioproc.h"
#include "procdef.h"
#include "reader.h"
#include "write.h"
#include "eval.h"
#include "repl.h"
#include "sform.h"


static int load_proc(object *params, object **result) {
    char *filename;
    FILE *in;
    object *exp, *val;
    object *env;

    if (result == NULL) {
        return SC_E_NULL;
    }
    if (!is_empty_list(cdr(params))) {
        return SC_E_ARITY;
    }
    exp = car(params);
    if (!is_string(exp)) {
        return SC_E_ARG_TYPE;
    }

    filename = obj_sv(exp);
    in = fopen(filename, "r");
    if (in == NULL) {
        return SC_E_IO_OPEN;
    }
    env = get_repl_env();
    val = get_nrv_symbol();
    while (!is_eof_object((exp = sc_read(in)))) {
        if (exp == NULL) {
            return SC_E_LOAD;
        }
        val = sc_eval(exp, env);
        if (val == NULL) {
            return SC_E_LOAD;
        }
    }
    fclose(in);
    *result = val;
    return 0;
}

static int open_input_port_proc(object *params, object **result) {
    char *filename;
    FILE *in;
    object *obj, *port;

    if (result == NULL) {
        return SC_E_NULL;
    }
    if (!is_empty_list(cdr(params))) {
        return SC_E_ARITY;
    }

    obj = car(params);
    if (!is_string(obj)) {
        return SC_E_ARG_TYPE;
    }
    filename = obj_sv(obj);
    in = fopen(filename, "r");
    if (in == NULL) {
        return SC_E_IO_OPEN;
    }
    port = make_input_port(in);
    if (port == NULL) {
        return SC_E_NO_MEM;
    }
    *result = port;
    return 0;
}

static int close_input_port_proc(object *params, object **result) {
    int ret;
    object *obj;

    if (result == NULL) {
        return SC_E_NULL;
    }
    if (!is_empty_list(cdr(params))) {
        return SC_E_ARITY;
    }
    
    obj = car(params);
    if (!is_input_port(obj)) {
        return SC_E_ARG_TYPE;
    }
    if (obj_ipv(obj) == NULL) {
        return SC_E_IO_INVL_PORT;
    }
    ret = fclose(obj_ipv(obj));
    obj_ipv(obj) = NULL;
    if (ret == EOF) {
        return SC_E_IO_CLOSE;
    }
    *result = get_nrv_symbol();
    return 0;
}

static int is_input_port_proc(object *params, object **result) {
    if (result == NULL) {
        return SC_E_NULL;
    }
    if (!is_empty_list(cdr(params))) {
        return SC_E_ARITY;
    }

    *result = is_input_port(car(params)) ?
                get_true_obj() : get_false_obj();
    return 0;
}

static int read_proc(object *params, object **result) {
    FILE *in;
    object *obj;

    if (result == NULL) {
        return SC_E_NULL;
    }
    if (!is_empty_list(params) && 
        !is_empty_list(cdr(params))) {
        return SC_E_ARITY;
    }

    if (is_empty_list(params)) {
        in = stdin;
    } else {
        obj = car(params);
        if (!is_input_port(obj)) {
            return SC_E_ARG_TYPE;
        }
        in = obj_ipv(obj);
        if (in == NULL) {
            return SC_E_IO_INVL_PORT;
        }
    }
    *result = sc_read(in);
    if (*result == NULL) {
        return SC_E_IO_READ;
    }
    return 0;
}

static int read_char_proc(object *params, object **result) {
    FILE *in;
    int ret;
    object *obj;

    if (result == NULL) {
        return SC_E_NULL;
    }
    if (!is_empty_list(params) && 
        !is_empty_list(cdr(params))) {
        return SC_E_ARITY;
    }

    if (is_empty_list(params)) {
        in = stdin;
        ret = getc(in);
        if (ret != '\n') {
            ungetc(ret, in);
        }
    } else {
        obj = car(params);
        if (!is_input_port(obj)) {
            return SC_E_ARG_TYPE;
        }
        in = obj_ipv(obj);
        if (in == NULL) {
            return SC_E_IO_INVL_PORT;
        }
    }
    ret = getc(in);
    *result = (ret == EOF) ?
                get_eof_object() : make_character(ret);
    if (*result == NULL) {
        return SC_E_IO_READ;
    }
    return 0;
}

static int peek_char_proc(object *params, object **result) {
    FILE *in;
    int ret;
    object *obj;

    if (result == NULL) {
        return SC_E_NULL;
    }
    if (!is_empty_list(params) && 
        !is_empty_list(cdr(params))) {
        return SC_E_ARITY;
    }

    if (is_empty_list(params)) {
        in = stdin;
        ret = getc(in);
        if (ret != '\n') {
            ungetc(ret, in);
        }
    } else {
        obj = car(params);
        if (!is_input_port(obj)) {
            return SC_E_ARG_TYPE;
        }
        in = obj_ipv(obj);
        if (in == NULL) {
            return SC_E_IO_INVL_PORT;
        }
    }
    ret = getc(in);
    ungetc(ret, in);
    *result = (ret == EOF) ? 
                get_eof_object() : make_character(ret);
    if (*result == NULL) {
        return SC_E_IO_READ;
    }
    return 0;
}

static int is_eof_object_proc(object *params, object **result) {
    if (result == NULL) {
        return SC_E_NULL;
    }
    if (!is_empty_list(cdr(params))) {
        return SC_E_ARITY;
    }

    *result = is_eof_object(car(params)) ?
                get_true_obj() : get_false_obj();
    return 0;
}

static int open_output_port_proc(object *params, object **result) {
    char *filename;
    FILE *out;
    object *obj;

    if (result == NULL) {
        return SC_E_NULL;
    }
    if (!is_empty_list(cdr(params))) {
        return SC_E_ARITY;
    }

    obj = car(params);
    if (!is_string(obj)) {
        return SC_E_ARG_TYPE;
    }
    filename = obj_sv(obj);
    out = fopen(filename, "a");
    if (out == NULL) {
        return SC_E_IO_OPEN;
    }
    *result = make_output_port(out);
    return 0;
}

static int close_output_port_proc(object *params, object **result) {
    int ret;
    object *obj;

    if (result == NULL) {
        return SC_E_NULL;
    }
    if (!is_empty_list(cdr(params))) {
        return SC_E_ARITY;
    }

    obj = car(params);
    if (!is_output_port(obj)) {
        return SC_E_ARG_TYPE;
    }
    if (obj_opv(obj) == NULL) {
        return SC_E_IO_INVL_PORT;
    }
    ret = fclose(obj_opv(obj));
    obj_opv(obj) = NULL;
    if (ret == EOF) {
        return SC_E_IO_CLOSE;
    }
    *result = get_nrv_symbol();
    return 0;
}

static int is_output_port_proc(object *params, object **result) {
    object *obj;

    if (result == NULL) {
        return SC_E_NULL;
    }
    if (!is_empty_list(cdr(params))) {
        return SC_E_ARITY;
    }

    obj = car(params);
    *result = (is_output_port(obj)) ?
                get_true_obj() : get_false_obj();
    return 0;
}

static int write_char_proc(object *params, object **result) {
    FILE *out;
    object *port, *ch;

    if (result == NULL) {
        return SC_E_NULL;
    }
    if (!is_empty_list(cdr(params)) &&
        !is_empty_list(cddr(params))) {
        return SC_E_ARITY;
    }

    ch = car(params);
    port = cadr(params);
    if (!is_character(ch)) {
        return SC_E_ARG_TYPE;
    }
    if (port == NULL) {
        out = stdout;
    } else if (is_output_port(port)){
        out = obj_opv(port);
    } else {
        return SC_E_ARG_TYPE;
    }
    if (out == NULL) {
        return SC_E_IO_INVL_PORT;
    }

    putc(obj_cv(ch), out);
    *result = get_nrv_symbol();
    return 0;
}

static int write_proc(object *params, object **result) {
    object *port, *obj;
    FILE *out;

    if (result == NULL) {
        return SC_E_NULL;
    }
    if (!is_empty_list(cdr(params)) &&
        !is_empty_list(cddr(params))) {
        return SC_E_ARITY;
    }

    obj = car(params);
    port = cadr(params);
    if (port == NULL) {
        out = stdout;
    } else if (is_output_port(port)){
        out = obj_opv(port);
    } else {
        return SC_E_ARG_TYPE;
    }
    if (out == NULL) {
        return SC_E_IO_INVL_PORT;
    }

    sc_write(out, obj);
    *result = get_nrv_symbol();
    return 0;
}

static int error_proc(object *params, object **result) {
    object *first, *rest;

    rest = params;
    while (!is_empty_list(rest)) {
        first = car(rest);
        sc_write(stderr, first);
        fprintf(stderr, " ");
        rest = cdr(rest);
    }
    fprintf(stderr, "\n");
    exit(1);
}


#define define_proc(x, y) \
    env_define_proc(x, y, env)

int init_io_primitive(object *env) {
    define_proc("load", load_proc);
    define_proc("open-input-port", open_input_port_proc);
    define_proc("close-input-port", close_input_port_proc);
    define_proc("input-port?", is_input_port_proc);
    define_proc("read", read_proc);
    define_proc("read-char", read_char_proc);
    define_proc("peek-char", peek_char_proc);
    define_proc("eof-object?", is_eof_object_proc);

    define_proc("write", write_proc);
    define_proc("write-char", write_char_proc);
    define_proc("output-port?", is_output_port_proc);
    define_proc("open-output-port", open_output_port_proc);
    define_proc("close-output-port", close_output_port_proc);
    define_proc("error", error_proc);
    return 0;
}

