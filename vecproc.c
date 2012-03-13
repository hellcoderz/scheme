#include "object.h"
#include "vecproc.h"
#include "procdef.h"
#include "objstream.h"
#include "mem.h"
#include "sform.h"
#include "gc.h"

static int is_vector_proc(object *params, object **result) {
    object *obj;

    check_null(result);
    check_arg1(params);
    obj = car(params);
    *result = is_vector(obj) ? get_true_obj() : get_false_obj();
    return 0;
}

static int make_vector_proc(object *params, object **result) {
    object *len, *fill;
    object **buf;
    long size;

    check_null(result);
    if (is_empty_list(params)) {
        return SC_E_ARITY;
    }
    len = car(params);
    if (!is_fixnum(len)) {
        return SC_E_ARG_TYPE;
    }
    size = obj_nv(len);
    buf = sc_malloc(sizeof(object*) * size);
    if (buf == NULL) {
        return SC_E_NO_MEM;
    }

    if (!is_empty_list(cdr(params))) {
        long i;
        if (!is_empty_list(cddr(params))) {
            return SC_E_ARITY;
        }
        fill = cadr(params);
        for (i = 0; i < size; i++) {
            buf[i] = fill;
        }
    }
    *result = make_vector(buf, size);
    if (*result == NULL) {
        return SC_E_NO_MEM;
    }
    return 0;
}

static int vector_proc(object *params, object **result) {
    object *obj, *rest;
    objstream *stream;
    object **buf;
    int ret;

    check_null(result);
    stream = objstream_new(-1);
    if (stream == NULL) {
        return SC_E_NO_MEM;
    }

    rest = params;
    while (!is_empty_list(rest)) {
        obj = car(rest);
        ret = objstream_append(stream, obj);
        if (ret != 0) {
            objstream_dispose(stream);
            return SC_E_NO_MEM;
        }
        rest = cdr(rest);
    }
    buf = objstream_trim(stream, &ret);
    *result = make_vector(buf, ret);
    objstream_dispose(stream);
    if (*result == NULL) {
        return SC_E_NO_MEM;
    }
    return 0;
}

static int vector_length_proc(object *params, object **result) {
    object *obj;

    check_null(result);
    check_arg1(params);

    obj = car(params);
    if (!is_vector(obj)) {
        return SC_E_ARG_TYPE;
    }
    *result = make_fixnum(obj_vsv(obj));
    if (*result == NULL) {
        return SC_E_NO_MEM;
    }
    return 0;
}

static int vector_ref_proc(object *params, object **result) {
    object *vec, *idx;
    object **buf;
    int size, i;

    check_null(result);
    check_arg2(params);

    vec = car(params);
    if (!is_vector(vec)) {
        return SC_E_ARG_TYPE;
    }
    idx = cadr(params);
    if (!is_fixnum(idx)) {
        return SC_E_ARG_TYPE;
    }
    i = (int)obj_nv(idx);
    size = obj_vsv(vec);
    if (i < 0 || i >= size) {
        return SC_E_INVL_INDEX;
    }
    buf = obj_vav(vec);
    *result = buf[i];
    return 0;
}

static int vector_set_proc(object *params, object **result) {
    object *vec, *idx, *obj;
    object **buf;
    int i, size;

    check_null(result);
    check_arg3(params);
    vec = car(params);
    if (!is_vector(vec)) {
        return SC_E_ARG_TYPE;
    }
    idx = cadr(params);
    if (!is_fixnum(idx)) {
        return SC_E_ARG_TYPE;
    }
    size = obj_vsv(vec);
    i = (int)obj_nv(idx);
    if (i < 0 || i >= size) {
        return SC_E_INVL_INDEX;
    }
    obj = caddr(params);
    buf = obj_vav(vec);
    buf[i] = obj;
    *result = get_nrv_symbol();
    return 0;
}

static int vector_to_list_proc(object *params, object **result) {
    object *vec, *list;
    object **buf;
    int size, i;

    check_null(result);
    check_arg1(params);
    vec = car(params);
    if (!is_vector(vec)) {
        return SC_E_ARG_TYPE;
    }
    buf = obj_vav(vec);
    size = obj_vsv(vec);
    list = get_empty_list();
    gc_protect(list);
    for (i = size - 1; i >= 0; i--) {
        list = cons(buf[i], list);
    }
    gc_abandon();
    *result = list;
    return 0;
}

static int list_to_vector_proc(object *params, object **result) {
    object *obj, *rest;
    objstream *stream;
    object **buf;
    int size;

    check_null(result);
    check_arg1(params);

    stream = objstream_new(-1);
    if (stream == NULL) {
        return SC_E_NO_MEM;
    }
    rest = car(params);
    if (!is_pair(rest) && !is_empty_list(rest)) {
        return SC_E_ARG_TYPE;
    }
    while (!is_empty_list(rest)) {
        obj = car(rest);
        size = objstream_append(stream, obj);
        if (size != 0) {
            objstream_dispose(stream);
            return SC_E_NO_MEM;
        }
        rest = cdr(rest);
    }
    buf = objstream_trim(stream, &size);
    *result = make_vector(buf, size);
    objstream_dispose(stream);
    if (*result == NULL) {
        return SC_E_NO_MEM;
    }
    return 0;
}

int init_vec_primitive(object *env) {
    define_proc("vector?", is_vector_proc);
    define_proc("make-vector", make_vector_proc);
    define_proc("vector", vector_proc);
    define_proc("vector-length", vector_length_proc);
    define_proc("vector-ref", vector_ref_proc);
    define_proc("vector-set!", vector_set_proc);
    define_proc("vector->list", vector_to_list_proc);
    define_proc("list->vector", list_to_vector_proc);

    return 0;
}
