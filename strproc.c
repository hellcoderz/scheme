#include <string.h>
#include "object.h"
#include "strproc.h"
#include "procdef.h"
#include "mem.h"
#include "sstream.h"

static int make_string_proc(object *params, object **result) {
    object *len_obj, *char_obj;
    char c;
    long len;
    char *buf;

    check_null(result);

    if (is_empty_list(params)) {
        return SC_E_ARITY;
    }

    len_obj = car(params);
    if (!is_fixnum(len_obj)) {
        return SC_E_ARG_TYPE;
    }
    len = obj_nv(len_obj);

    if (is_empty_list(cdr(params))) {
        c = 'x';
    } else if (is_empty_list(cddr(params))) {
        char_obj = cadr(params);
        if (!is_character(char_obj)) {
            return SC_E_ARG_TYPE;
        }
        c = obj_cv(char_obj);
    } else {
        return SC_E_ARITY;
    }

    buf = sc_malloc(len + 1);
    memset(buf, c, len);
    buf[len] = '\0';
    *result = make_string(buf);
    sc_free(buf);
    if (*result == NULL) {
        return SC_E_NO_MEM;
    }

    return 0;
}

static int string_proc(object *params, object **result) {
    sstream *stream;
    object *obj, *rest;
    char *str;
    int ret;

    check_null(result);
    if (is_empty_list(params)) {
        return SC_E_ARITY;
    }

    stream = sstream_new(-1);
    if (stream == NULL) {
        return SC_E_NO_MEM;
    }

    rest = params;
    while (!is_empty_list(rest)) {
        obj = car(rest);
        if (!is_character(obj)) {
            sstream_dispose(stream);
            return SC_E_ARG_TYPE;
        }
        ret = sstream_append(stream, obj_cv(obj));
        if (ret != 0) {
            sstream_dispose(stream);
            return SC_E_NO_MEM;
        }
        rest = cdr(rest);
    }
    
    str = sstream_cstr(stream);
    if (str == NULL) {
        sstream_dispose(stream);
        return SC_E_NO_MEM;
    }
    *result = make_string(str);
    sc_free(str);
    if (*result == NULL) {
        return SC_E_NO_MEM;
    }
    sstream_dispose(stream);
    return 0;
}

static int string_length_proc(object *params, object **result) {
    object *obj;

    check_null(result);
    check_arg1(params);
    obj = car(params);
    if (!is_string(obj)) {
        return SC_E_ARG_TYPE;
    }
    *result = make_fixnum(obj_slenv(obj));
    if (*result == NULL) {
        return SC_E_NO_MEM;
    }
    return 0;
}

static int string_ref_proc(object *params, object **result) {
    int i, len;
    object *obj, *idx_obj;
    char *str;

    check_null(result);
    check_arg2(params);

    obj = car(params);
    if (!is_string(obj)) {
        return SC_E_ARG_TYPE;
    }
    idx_obj = cadr(params);
    if (!is_fixnum(idx_obj)) {
        return SC_E_ARG_TYPE;
    }
    
    len = obj_slenv(obj);
    i = (int)obj_nv(idx_obj);
    if (i < 0 || i >= len) {
        return SC_E_INVL_INDEX;
    }
    str = obj_sv(obj);
    *result = make_character(str[i]);
    if (*result == NULL) {
        return SC_E_NO_MEM;
    }
    return 0;
}

static int string_set_proc(object *params, object **result) {
    object *str_obj, *idx_obj, *char_obj;
    char *str;
    int i, len;

    check_null(result);
    check_arg3(params);

    str_obj = car(params);
    if (!is_string(str_obj)) {
        return SC_E_ARG_TYPE;
    }
    idx_obj = cadr(params);
    if (!is_fixnum(idx_obj)) {
        return SC_E_ARG_TYPE;
    }
    char_obj = caddr(params);
    if (!is_character(char_obj)) {
        return SC_E_ARG_TYPE;
    }
    
    len = obj_slenv(str_obj);
    i = (int)obj_nv(idx_obj);
    if (i < 0 || i >= len) {
        return SC_E_INVL_INDEX;
    }

    str = string_remove(str_obj);
    str[i] = obj_cv(char_obj);
    *result = string_insert(str_obj);
    return 0;
}

int init_str_primitive(object *env) {
    define_proc("make-string", make_string_proc);
    define_proc("string", string_proc);
    define_proc("string-length", string_length_proc);
    define_proc("string-ref", string_ref_proc);
    define_proc("string-set!", string_set_proc);

    return 0;
}

