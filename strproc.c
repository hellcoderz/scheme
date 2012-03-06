#include <string.h>
#include "object.h"
#include "strproc.h"
#include "procdef.h"
#include "mem.h"
#include "sstream.h"
#include "gc.h"
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

    str[i] = obj_cv(char_obj);
    *result = str_obj;
    return 0;
}

static int compare_string(object *a, object *b) {
    char *p, *q;

    p = obj_sv(a);
    q = obj_sv(b);

    return strcmp(p, q);
}

static int string_is_equal_proc(object *params, object **result) {
    object *a, *b;
    int ret;

    check_null(result);
    check_arg2(params);

    a = car(params);
    b = cadr(params);
    if (!is_string(a) || !is_string(b)) {
        return SC_E_ARG_TYPE;
    }

    ret = compare_string(a, b);
    if (ret == 0) {
        *result = get_true_obj();
    } else {
        *result = get_false_obj();
    }
    return 0;
}

static int string_is_greater_proc(object *params, object **result) {
    object *a, *b;
    int ret;

    check_null(result);
    check_arg2(params);

    a = car(params);
    b = cadr(params);
    if (!is_string(a) || !is_string(b)) {
        return SC_E_ARG_TYPE;
    }

    ret = compare_string(a, b);
    if (ret > 0) {
        *result = get_true_obj();
    } else {
        *result = get_false_obj();
    }
    return 0;
}

static int string_is_less_proc(object *params, object **result) {
    object *a, *b;
    int ret;

    check_null(result);
    check_arg2(params);

    a = car(params);
    b = cadr(params);
    if (!is_string(a) || !is_string(b)) {
        return SC_E_ARG_TYPE;
    }

    ret = compare_string(a, b);
    if (ret < 0) {
        *result = get_true_obj();
    } else {
        *result = get_false_obj();
    }
    return 0;
}

static int string_is_not_less_proc(object *params, object **result) {
    object *a, *b;
    int ret;

    check_null(result);
    check_arg2(params);

    a = car(params);
    b = cadr(params);
    if (!is_string(a) || !is_string(b)) {
        return SC_E_ARG_TYPE;
    }

    ret = compare_string(a, b);
    if (ret >= 0) {
        *result = get_true_obj();
    } else {
        *result = get_false_obj();
    }
    return 0;
}

static int string_is_not_greater_proc(object *params, object **result) {
    object *a, *b;
    int ret;

    check_null(result);
    check_arg2(params);

    a = car(params);
    b = cadr(params);
    if (!is_string(a) || !is_string(b)) {
        return SC_E_ARG_TYPE;
    }

    ret = compare_string(a, b);
    if (ret <= 0) {
        *result = get_true_obj();
    } else {
        *result = get_false_obj();
    }
    return 0;
}

static int compare_string_ci(object *a, object *b) {
    char *p, *q;

    p = obj_sv(a);
    q = obj_sv(b);
    return strcasecmp(p, q);
}

static int string_ci_equal_proc(object *params, object **result) {
    object *a, *b;
    int ret;

    check_null(result);
    check_arg2(params);

    a = car(params);
    b = cadr(params);
    if (!is_string(a) || !is_string(b)) {
        return SC_E_ARG_TYPE;
    }

    ret = compare_string_ci(a, b);
    if (ret == 0) {
        *result = get_true_obj();
    } else {
        *result = get_false_obj();
    }
    return 0;
}

static int string_ci_greater_proc(object *params, object **result) {
    object *a, *b;
    int ret;

    check_null(result);
    check_arg2(params);

    a = car(params);
    b = cadr(params);
    if (!is_string(a) || !is_string(b)) {
        return SC_E_ARG_TYPE;
    }

    ret = compare_string_ci(a, b);
    if (ret > 0) {
        *result = get_true_obj();
    } else {
        *result = get_false_obj();
    }
    return 0;
}

static int string_ci_less_proc(object *params, object **result) {
    object *a, *b;
    int ret;

    check_null(result);
    check_arg2(params);

    a = car(params);
    b = cadr(params);
    if (!is_string(a) || !is_string(b)) {
        return SC_E_ARG_TYPE;
    }

    ret = compare_string_ci(a, b);
    if (ret < 0) {
        *result = get_true_obj();
    } else {
        *result = get_false_obj();
    }
    return 0;
}

static int string_ci_not_less_proc(object *params, object **result) {
    object *a, *b;
    int ret;

    check_null(result);
    check_arg2(params);

    a = car(params);
    b = cadr(params);
    if (!is_string(a) || !is_string(b)) {
        return SC_E_ARG_TYPE;
    }

    ret = compare_string_ci(a, b);
    if (ret >= 0) {
        *result = get_true_obj();
    } else {
        *result = get_false_obj();
    }
    return 0;
}

static int string_ci_not_greater_proc(object *params, object **result) {
    object *a, *b;
    int ret;

    check_null(result);
    check_arg2(params);

    a = car(params);
    b = cadr(params);
    if (!is_string(a) || !is_string(b)) {
        return SC_E_ARG_TYPE;
    }

    ret = compare_string_ci(a, b);
    if (ret <= 0) {
        *result = get_true_obj();
    } else {
        *result = get_false_obj();
    }
    return 0;
}

static int substring_proc(object *params, object **result) {
    object *str_obj, *s_obj, *e_obj;
    int start, end, len, buflen;
    char *str, *buf;

    check_null(result);
    check_arg3(params);

    str_obj = car(params);
    if (!is_string(str_obj)) {
        return SC_E_ARG_TYPE;
    }
    str = obj_sv(str_obj);
    len = obj_slenv(str_obj);
    
    s_obj = cadr(params);
    e_obj = caddr(params);
    if (!is_fixnum(s_obj) || !is_fixnum(e_obj)) {
        return SC_E_ARG_TYPE;
    }
    start = (int)obj_nv(s_obj);
    end = (int)obj_nv(e_obj);
    if (start < 0 || end > len || start > end) {
        return SC_E_ARG_INVL;
    }
    
    buflen = end - start;
    buf = sc_malloc(buflen + 1);
    if (buf == NULL) {
        return SC_E_NO_MEM;
    }
    strncpy(buf, str, buflen); /* no NUL at end */
    buf[buflen] = '\0';
    *result = make_string(buf);
    sc_free(buf);
    if (*result == NULL) {
        return SC_E_NO_MEM;
    }
    return 0;
}

static int string_copy_proc(object *params, object **result) {
    object *src;

    check_null(result);
    check_arg1(params);

    src = car(params);
    if (!is_string(src)) {
        return SC_E_ARG_TYPE;
    }
    *result = make_string(obj_sv(src));
    if (*result == NULL) {
        return SC_E_NO_MEM;
    }
    return 0;
}

static int string_fill_proc(object *params, object **result) {
    object *str, *ch;

    check_null(result);
    check_arg2(params);

    str = car(params);
    ch = cadr(params);
    if (!is_string(str) || !is_character(ch)) {
        return SC_E_ARG_TYPE;
    }
    memset(obj_sv(str), obj_cv(ch), obj_slenv(str));
    *result = str;
    return 0;
}

static int total_length(object *seq) {
    object *elem, *rest;
    int len = 0;

    rest = seq;
    while (!is_empty_list(rest)) {
        elem = car(rest);
        if (!is_string(elem)) {
            return -1;
        }
        len += obj_slenv(elem);
        rest = cdr(rest);
    }
    return len;
}

static int string_append_proc(object *params, object **result) {
    object *rest, *elem;
    int len;
    char *buf;

    check_null(result);
    if (is_empty_list(params)) {
        return SC_E_ARITY;
    }

    len = total_length(params);
    if (len == -1) {
        return SC_E_ARG_TYPE;
    }
    buf = sc_malloc(len + 1);
    if (buf == NULL) {
        return SC_E_NO_MEM;
    }
    buf[0] = '\0';

    rest = params;
    while (!is_empty_list(rest)) {
        elem = car(rest);
        strcat(buf, obj_sv(elem));
        rest = cdr(rest);
    }

    *result = make_string(buf);
    if (*result == NULL) {
        return SC_E_NO_MEM;
    }
    return 0;
}

static int string_to_list_proc(object *params, object **result) {
    object *str_obj, *list, *ch;
    char *str;
    int len, i;

    check_null(result);
    check_arg1(params);

    str_obj = car(params);
    if (!is_string(str_obj)) {
        return SC_E_ARG_TYPE;
    }
    str = obj_sv(str_obj);
    len = obj_slenv(str_obj);
    list = get_empty_list();

    ch = make_character(str[len-1]);
    gc_protect(ch);
    list = cons(ch, list);
    gc_protect(list);
    for (i = len - 2; i >= 0; i--) {
        ch = make_character(str[i]);
        list = cons(ch, list);
    }
    gc_abandon();
    gc_abandon();
    *result = list;
    return 0;
}

static int list_to_string_proc(object *params, object **result) {
    object *list, *ch;
    sstream *stream;
    char *str;
    int ret;

    check_null(result);
    check_arg1(params);

    list = car(params);
    if (!is_pair(list)) {
        return SC_E_ARG_TYPE;
    }

    stream = sstream_new(-1);
    if (stream == NULL) {
        return SC_E_NO_MEM;
    }
    while (!is_empty_list(list)) {
        ch = car(list);
        if (!is_character(ch)) {
            sstream_dispose(stream);
            return SC_E_ARG_TYPE;
        }
        ret = sstream_append(stream, obj_cv(ch));
        if (ret != 0) {
            sstream_dispose(stream);
            return SC_E_NO_MEM;
        }
        list = cdr(list);
        if (!is_pair(list) && !is_empty_list(list)) {
            sstream_dispose(stream);
            return SC_E_ARG_TYPE;
        }
    }

    str = sstream_cstr(stream);
    if (str == NULL) {
        sstream_dispose(stream);
        return SC_E_NO_MEM;
    }
    *result = make_string(str);
    sc_free(str);
    sstream_dispose(stream);
    if (*result == NULL) {
        return SC_E_NO_MEM;
    }
    return 0;
}

int init_str_primitive(object *env) {
    define_proc("make-string", make_string_proc);
    define_proc("string", string_proc);
    define_proc("string-length", string_length_proc);
    define_proc("string-ref", string_ref_proc);
    define_proc("string-set!", string_set_proc);
    define_proc("string=?", string_is_equal_proc);
    define_proc("string-ci=?", string_ci_equal_proc);
    define_proc("string<?", string_is_less_proc);
    define_proc("string>?", string_is_greater_proc);
    define_proc("string<=?", string_is_not_greater_proc);
    define_proc("string>=?", string_is_not_less_proc);
    define_proc("string-ci<?", string_ci_less_proc);
    define_proc("string-ci>?", string_ci_greater_proc);
    define_proc("string-ci<=?", string_ci_not_greater_proc);
    define_proc("string-ci>=?", string_ci_not_less_proc);
    define_proc("substring", substring_proc);
    define_proc("string-copy", string_copy_proc);
    define_proc("string-fill!", string_fill_proc);
    define_proc("string-append", string_append_proc);
    define_proc("string->list", string_to_list_proc);
    define_proc("list->string", list_to_string_proc);

    return 0;
}

