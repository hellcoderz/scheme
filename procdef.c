#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include "object.h"
#include "procdef.h"
#include "env.h"
#include "sform.h"
#include "repl.h"
#include "ioproc.h"
#include "mathproc.h"
#include "gc.h"

int env_define_proc(char *sym, prim_proc fn, object *env) {
    object *sym_obj, *proc_obj;
    int result;

    sym_obj = make_symbol(sym);
    gc_protect(sym_obj);
    proc_obj = make_primitive_proc(fn);
    gc_protect(proc_obj);
    result = define_variable(sym_obj, proc_obj, env);
    gc_abandon();
    gc_abandon();
    return result;
}

char* error_str(int err) {
    switch (err) {
        case SC_E_ARG_TYPE:
            return "bad argument type";
        case SC_E_NO_MEM:
            return "no memory";
        case SC_E_NULL:
            return "null pointer";
        case SC_E_ARITY:
            return "wrong arity";
        case SC_E_ARG_INVL:
            return "invalid parameter";
        case SC_E_DIV:
            return "divide by zero";
        case SC_E_INV_STAT:
            return "illegal state";
        case SC_E_IO_OPEN:
            return "error while opening file";
        case SC_E_LOAD:
            return "syntax error while loading source file";
        case SC_E_IO_CLOSE:
            return "error while closing file";
        case SC_E_IO_READ:
            return "error while reading file";
        case SC_E_IO_INVL_PORT:
            return "invalid I/O port";
    }
    return NULL;
}


static int is_null_proc(object *params, object **result) {
    int ret;

    if (result == NULL) {
        return SC_E_NULL;
    }
    if (!is_empty_list(cdr(params))) {
        return SC_E_ARITY;
    }

    ret = is_empty_list(car(params));
    *result = ret ? get_true_obj() : get_false_obj();
    return 0;
}

static int is_boolean_proc(object *params, object **result) {
   int ret;

    if (result == NULL) {
        return SC_E_NULL;
    }
    if (!is_empty_list(cdr(params))) {
        return SC_E_ARITY;
    }

    ret = is_boolean(car(params));
    *result = ret ? get_true_obj() : get_false_obj();
    return 0;
}

static int is_symbol_proc(object *params, object **result) {
    int ret;

    if (result == NULL) {
        return SC_E_NULL;
    }
    if (!is_empty_list(cdr(params))) {
        return SC_E_ARITY;
    }

    ret = is_symbol(car(params));
    *result = ret ? get_true_obj() : get_false_obj();
    return 0;
}

static int is_integer_proc(object *params, object **result) {
    int ret;

    if (result == NULL) {
        return SC_E_NULL;
    }
    if (!is_empty_list(cdr(params))) {
        return SC_E_ARITY;
    }

    ret = is_fixnum(car(params));
    *result = ret ? get_true_obj() : get_false_obj();
    return 0;
}

static int is_real_proc(object *params, object **result) {
    int ret;
    object *obj;

    if (result == NULL) {
        return SC_E_NULL;
    }
    if (!is_empty_list(cdr(params))) {
        return SC_E_ARITY;
    }

    obj = car(params);
    ret = is_flonum(obj) || is_fixnum(obj);
    *result = ret ? get_true_obj() : get_false_obj();
    return 0;
}

static int is_char_proc(object *params, object **result) {
    int ret;

    if (result == NULL) {
        return SC_E_NULL;
    }
    if (!is_empty_list(cdr(params))) {
        return SC_E_ARITY;
    }

    ret = is_character(car(params));
    *result = ret ? get_true_obj() : get_false_obj();
    return 0;
}

static int is_string_proc(object *params, object **result) {
    int ret;
    
    if (result == NULL) {
        return SC_E_NULL;
    }
    if (!is_empty_list(cdr(params))) {
        return SC_E_ARITY;
    }

    ret = is_string(car(params));
    *result = ret ? get_true_obj() : get_false_obj();
    return 0;
}

static int is_pair_proc(object *params, object **result) {
    int ret;

    if (result == NULL) {
        return SC_E_NULL;
    }
    if (!is_empty_list(cdr(params))) {
        return SC_E_ARITY;
    }

    ret = is_pair(car(params));
    *result = ret ? get_true_obj() : get_false_obj();
    return 0;
}

static int is_procedure_proc(object *params, object **result) {
    int ret;
    object *obj;

    if (result == NULL) {
        return SC_E_NULL;
    }
    if (!is_empty_list(cdr(params))) {
        return SC_E_ARITY;
    }

    obj = car(params);
    ret = is_primitive_proc(obj) || is_compound_proc(obj);
    *result = ret ? get_true_obj() : get_false_obj();
    return 0;
}

static int char_to_integer_proc(object *params, object **result) {
    if (result == NULL) {
        return SC_E_NULL;
    }
    if (!is_empty_list(cdr(params))) {
        return SC_E_ARITY;
    }
    if (!is_character(car(params))) {
        return SC_E_ARG_TYPE;
    }

    *result = make_fixnum(obj_cv(car(params)));
    if (*result == NULL) {
        return SC_E_NO_MEM;
    }
    return 0;
}

static int integer_to_char_proc(object *params, object **result) {
    long i;

    if (result == NULL) {
        return SC_E_NULL;
    }
    if (!is_empty_list(cdr(params))) {
        return SC_E_ARITY;
    }
    if (!is_fixnum(car(params))) {
        return SC_E_ARG_TYPE;
    }

    i = obj_nv(car(params));
    if (!isascii(i)) {
        return SC_E_ARG_INVL;
    }
    *result =make_character(i);
    if (*result == NULL) {
        return SC_E_NO_MEM;
    }
    return 0;
}

static int number_to_string_proc(object *params, object **result) {
    char buf[64];
    object *num_obj;

    if (result == NULL) {
        return SC_E_NULL;
    }
    if (!is_empty_list(cdr(params))) {
        return SC_E_ARITY;
    }

    num_obj = car(params);
    if (is_fixnum(num_obj)) {
        sprintf(buf, "%ld", obj_nv(num_obj));
    } else if (is_flonum(num_obj)) {
        sprintf(buf, "%.8g", obj_rv(num_obj));
    } else {
        return SC_E_ARG_TYPE;
    }

    *result = make_string(buf);
    if (*result == NULL) {
        return SC_E_NO_MEM;
    }
    return 0;
}

static int string_to_number_proc(object *params, object **result) {
    long lv;
    double dv;
    char is_double = 0;
    char *str;
    object *str_obj;

    if (result == NULL) {
        return SC_E_NULL;
    }
    if (!is_empty_list(cdr(params))) {
        return SC_E_ARITY;
    }
    if (!is_string(car(params))) {
        return SC_E_ARG_TYPE;
    }
    
    str = obj_sv(car(params));
    if (*str == '-' || *str == '+') {
        str++;
    }
    while (*str != '\0' && isdigit(*str)) {
        str++;
    }
    if (*str == '.' && isdigit(*(++str))) {
        is_double = 1;
        while (*str != '\0' && isdigit(*str)) {
            str++;
        }
    }
    if (*str != '\0') {
        return SC_E_ARG_INVL;
    }

    str_obj = car(params);
    if (is_double) {
        dv = atof(obj_sv(str_obj));
        *result = make_flonum(dv);
    } else {
        lv = atol(obj_sv(str_obj));
        *result = make_fixnum(lv);
    }
    if (*result == NULL) {
        return SC_E_NO_MEM;
    }
    return 0;
}

static int symbol_to_string_proc(object *params, object **result) {
    if (result == NULL) {
        return SC_E_NULL;
    }
    if (!is_empty_list(cdr(params))) {
        return SC_E_ARITY;
    }
    if (!is_symbol(car(params))) {
        return SC_E_ARG_TYPE;
    }

    *result = make_string(obj_iv(car(params)));
    if (*result == NULL) {
        return SC_E_NO_MEM;
    }
    return 0;
}

static int exact_to_inexact_proc(object *params, object **result) {
    object *obj;
    double d;

    check_null(result);
    check_arg1(params);

    obj = car(params);
    if (!is_fixnum(obj)) {
        return SC_E_ARG_TYPE;
    }
    d = (double)obj_nv(obj);
    *result = make_flonum(d);
    if (*result == NULL) {
        return SC_E_NO_MEM;
    }
    return 0;
}

static int inexact_to_exact_proc(object *params, object **result) {
    object *obj;
    long n;

    check_null(result);
    check_arg1(params);

    obj = car(params);
    if (!is_flonum(obj)) {
        return SC_E_ARG_TYPE;
    }
    n = (long)obj_rv(obj);
    *result = make_fixnum(n);
    if (*result == NULL) {
        return SC_E_NO_MEM;
    }
    return 0;
}

static int string_to_symbol_proc(object *params, object **result) {
    if (result == NULL) {
        return SC_E_NULL;
    }
    if (!is_empty_list(cdr(params))) {
        return SC_E_ARITY;
    }
    if (!is_string(car(params))) {
        return SC_E_ARG_TYPE;
    }

    *result = make_symbol(obj_sv(car(params)));
    if (*result == NULL) {
        return SC_E_NO_MEM;
    }
    return 0;
}

double number_to_double(object *num_obj) {
    if (is_fixnum(num_obj)) {
        return (double)obj_nv(num_obj);
    } else if (is_flonum(num_obj)) {
        return obj_rv(num_obj);
    } else {
        return 0.0;
    }
}

int is_number(object *obj) {
    return is_fixnum(obj) || is_flonum(obj);
}

static int add_proc(object *params, object **result) {
    long sum = 0;
    double dsum = 0.0;
    char use_double = 0;
    object *list = params;
    object *elem;

    if (result == NULL) {
        return SC_E_NULL;
    }

    while (!is_empty_list(list)) {
        elem = car(list);
        if (is_flonum(elem)) {
            use_double = 1;
            break;
        } else if (!is_fixnum(elem)) {
            return SC_E_ARG_TYPE;
        }
        list = cdr(list);
    }
    
    list = params;
    if (use_double) {
        while (!is_empty_list(list)) {
            elem = car(list);
            if (is_number(elem)) {
                dsum += number_to_double(elem);
                list = cdr(list);
            } else {
                return SC_E_ARG_TYPE;
            }
        }
        *result = make_flonum(dsum);
    } else {
        while (!is_empty_list(list)) {
            elem = car(list);
            if (is_fixnum(elem)) {
                sum += obj_nv(elem);
                list = cdr(list);
            } else {
                return SC_E_ARG_TYPE;
            }
        }
        *result = make_fixnum(sum);
    }
    if (*result == NULL) {
        return SC_E_NO_MEM;
    }
    return 0;
}

static int sub_proc(object *params, object **result) {
    long remain;
    double dremain;
    char use_double = 0;
    object *list;
    object *elem;

    if (result == NULL) {
        return SC_E_NULL;
    }
    if (is_empty_list(params)) {
        return SC_E_ARITY;
    }

    list = params;
    while (!is_empty_list(list)) {
        elem = car(list);
        if (is_flonum(elem)) {
            use_double = 1;
            break;
        } else if (!is_fixnum(elem)) {
            return SC_E_ARG_TYPE;
        }
        list = cdr(list);
    }
   
    elem = car(params);
    if (!is_number(elem)) {
        return SC_E_ARG_TYPE;
    }

    list = cdr(params);
    if (use_double) {
        dremain = number_to_double(elem);
        if (is_empty_list(list)) {
            /* one argument: (- x) = -x */
            dremain = -dremain;
        } else {
            while (!is_empty_list(list)) {
                elem = car(list);
                if (is_number(elem)) {
                    dremain -= number_to_double(elem);
                    list = cdr(list);
                } else {
                    return SC_E_ARG_TYPE;
                }
            }
        }
        *result = make_flonum(dremain);
    } else {
        remain = obj_nv(elem);
        if (is_empty_list(list)) {
            /* one argument: (- x) = -x */
            remain = -remain;
        } else {
            while (!is_empty_list(list)) {
                elem = car(list);
                if (is_fixnum(elem)) {
                    remain -= obj_nv(elem);
                    list = cdr(list);
                } else {
                    return SC_E_ARG_TYPE;
                }
            }
        }
        *result = make_fixnum(remain);
    }
    if (*result == NULL) {
        return SC_E_NO_MEM;
    }
    return 0;
}

static int div_proc(object *params, object **result) {
    double n, d, ret;
    object *nobj, *dobj;

    if (result == NULL) {
        return SC_E_NULL;
    }
    if (!is_empty_list(cddr(params))) {
        return SC_E_ARITY;
    }
    nobj = car(params);
    dobj = cadr(params);
    if (!is_number(nobj) || !is_number(dobj)) {
        return SC_E_ARG_TYPE;
    }

    n = number_to_double(nobj);
    d = number_to_double(dobj);
    if (d == 0) {
        return SC_E_DIV;
    }
    ret = n / d;
    
    *result = make_flonum(ret);
    if (*result == NULL) {
        return SC_E_NO_MEM;
    }
    return 0;
}

static int mul_proc(object *params, object **result) {
    long product = 1;
    double dproduct = 1.0;
    char use_double = 0;
    object *list = params;
    object *elem;

    if (result == NULL) {
        return SC_E_NULL;
    }

    while (!is_empty_list(list)) {
        elem = car(list);
        if (is_flonum(elem)) {
            use_double = 1;
            break;
        } else if (!is_fixnum(elem)) {
            return SC_E_ARG_TYPE;
        }
        list = cdr(list);
    }
    
    list = params;
    if (use_double) {
        while (!is_empty_list(list)) {
            elem = car(list);
            if (is_number(elem)) {
                dproduct *= number_to_double(elem);
                list = cdr(list);
            } else {
                return SC_E_ARG_TYPE;
            }
        }
        *result = make_flonum(dproduct);
    } else {
        while (!is_empty_list(list)) {
            elem = car(list);
            if (is_fixnum(elem)) {
                product *= obj_nv(elem);
                list = cdr(list);
            } else {
                return SC_E_ARG_TYPE;
            }
        }
        *result = make_fixnum(product);
    }
    if (*result == NULL) {
        return SC_E_NO_MEM;
    }
    return 0; 
}

static int quotient_proc(object *params, object **result) {
    long quotient;
    long a, b;
    object *obj_a, *obj_b;

    if (result == NULL) {
        return SC_E_NULL;
    }
    if (!is_empty_list(cddr(params))) {
        return SC_E_ARITY;
    }
    
    obj_a = car(params);
    obj_b = cadr(params);
    if (!is_fixnum(obj_a) || !is_fixnum(obj_b)) {
        return SC_E_ARG_TYPE;
    }
    a = obj_nv(obj_a);
    b = obj_nv(obj_b);
    if (b == 0) {
        return SC_E_DIV;
    }
    quotient = a / b;
    *result = make_fixnum(quotient);
    if (*result == NULL) {
        return SC_E_NO_MEM;
    }
    return 0;
}

static int remainder_proc(object *params, object **result) {
    long remainder;
    long a, b;
    object *obj_a, *obj_b;

    if (result == NULL) {
        return SC_E_NULL;
    }
    if (!is_empty_list(cddr(params))) {
        return SC_E_ARITY;
    }
    
    obj_a = car(params);
    obj_b = cadr(params);
    if (!is_fixnum(obj_a) || !is_fixnum(obj_b)) {
        return SC_E_ARG_TYPE;
    }
    a = obj_nv(obj_a);
    b = obj_nv(obj_b);
    if (b == 0) {
        return SC_E_DIV;
    }
    remainder = a % b;
    *result = make_fixnum(remainder);
    if (*result == NULL) {
        return SC_E_NO_MEM;
    }
    return 0;
}

typedef enum {EQ, LESS, GREATER, LESS_EQ, GREATER_EQ} cmp_mode;

static int number_compare(object *params, object **result, cmp_mode mode) {
    object *false_obj;
    double val;
    object *list, *elem;

    if (result == NULL) {
        return SC_E_NULL;
    }
    if (is_empty_list(cdr(params)) ||
        is_empty_list(params)) {
        return SC_E_ARITY;
    }

    list = cdr(params);
    elem = car(params);
    if (!is_number(elem)) {
        return SC_E_ARG_TYPE;
    }
    val = number_to_double(elem);
    false_obj = get_false_obj();
    while (!is_empty_list(list)) {
        elem = car(list);
        if (!is_number(elem)) {
            return SC_E_ARG_TYPE;
        }
        switch (mode) {
            case EQ:
                if (val != number_to_double(elem)) {
                    *result = false_obj;
                    return 0;
                }
                break;
            case LESS:
                if (val >= number_to_double(elem)) {
                    *result = false_obj;
                    return 0;
                }
                break;
            case GREATER:
                if (val <= number_to_double(elem)) {
                    *result = false_obj;
                    return 0;
                }
                break;
            case LESS_EQ:
                if (val > number_to_double(elem)) {
                    *result = false_obj;
                    return 0;
                }
                break;
            case GREATER_EQ:
                if (val < number_to_double(elem)) {
                    *result = false_obj;
                    return 0;
                }
                break;
            default:
                *result = false_obj;
                return 0;
        }
        val = number_to_double(elem);
        list = cdr(list);
    }
    *result = get_true_obj();
    return 0;
}

static int is_number_equal_proc(object *params, object **result) {
    return number_compare(params, result, EQ);
}

static int is_number_less_proc(object *params, object **result) {
    return number_compare(params, result, LESS);
}

static int is_number_greater_proc(object *params, object **result) {
    return number_compare(params, result, GREATER);
}

static int is_number_less_equal_proc(object *params, object **result) {
    return number_compare(params, result, LESS_EQ);
}

static int is_number_greater_equal_proc(object *params, object **result) {
    return number_compare(params, result, GREATER_EQ);
}

static int cons_proc(object *params, object **result) {
    if (result == NULL) {
        return SC_E_NULL;
    }
    if (!is_empty_list(cddr(params))) {
        return SC_E_ARITY;
    }

    if (set_cdr(params, cadr(params)) != 0) {
        return SC_E_ARG_TYPE;
    }
    *result = params;
    return 0;
}

static int car_proc(object *params, object **result) {
    object *pair;

    if (result == NULL) {
        return SC_E_NULL;
    }
    if (!is_empty_list(cdr(params))) {
        return SC_E_ARITY;
    }
    pair = car(params);
    if (!is_pair(pair)) {
        return SC_E_ARG_TYPE;
    }
    *result = car(pair);
    return 0;
}

static int cdr_proc(object *params, object **result) {
    object *pair;

    if (result == NULL) {
        return SC_E_NULL;
    }
    if (!is_empty_list(cdr(params))) {
        return SC_E_ARITY;
    }
    pair = car(params);
    if (!is_pair(pair)) {
        return SC_E_ARG_TYPE;
    }
    *result = cdr(pair);
    return 0;
}

#define DEFINE_LIST_OP(name) \
    static int name ## _proc(object *params, object **result) { \
        if (result == NULL) { \
            return SC_E_NULL; \
        } \
        if (!is_empty_list(cdr(params))) { \
            return SC_E_ARITY; \
        } \
        *result = name(car(params)); \
        if (*result == NULL) { \
            return SC_E_ARG_INVL; \
        } \
        return 0; \
    }

DEFINE_LIST_OP(caar)
DEFINE_LIST_OP(cadr)
DEFINE_LIST_OP(cdar)
DEFINE_LIST_OP(cddr)
DEFINE_LIST_OP(caaar)
DEFINE_LIST_OP(caadr)
DEFINE_LIST_OP(cadar)
DEFINE_LIST_OP(caddr)
DEFINE_LIST_OP(cdaar)
DEFINE_LIST_OP(cdadr)
DEFINE_LIST_OP(cddar)
DEFINE_LIST_OP(cdddr)
DEFINE_LIST_OP(caaaar)
DEFINE_LIST_OP(caaadr)
DEFINE_LIST_OP(caadar)
DEFINE_LIST_OP(caaddr)
DEFINE_LIST_OP(cadaar)
DEFINE_LIST_OP(cadadr)
DEFINE_LIST_OP(caddar)
DEFINE_LIST_OP(cadddr)
DEFINE_LIST_OP(cdaaar)
DEFINE_LIST_OP(cdaadr)
DEFINE_LIST_OP(cdadar)
DEFINE_LIST_OP(cdaddr)
DEFINE_LIST_OP(cddaar)
DEFINE_LIST_OP(cddadr)
DEFINE_LIST_OP(cdddar)
DEFINE_LIST_OP(cddddr)

static int set_car_proc(object *params, object **result) {
    object *pair, *car_obj;

    if (result == NULL) {
        return SC_E_NULL;
    }
    if (!is_empty_list(cddr(params))) {
        return SC_E_ARITY;
    }
    
    pair = car(params);
    car_obj = cadr(params);
    if (set_car(pair, car_obj) != 0) {
        return SC_E_ARG_TYPE;
    }
    *result = get_nrv_symbol();
    return 0;
}

static int set_cdr_proc(object *params, object **result) {
    object *pair, *car_obj;

    if (result == NULL) {
        return SC_E_NULL;
    }
    if (!is_empty_list(cddr(params))) {
        return SC_E_ARITY;
    }
    
    pair = car(params);
    car_obj = cadr(params);
    if (set_cdr(pair, car_obj) != 0) {
        return SC_E_ARG_TYPE;
    }
    *result = get_nrv_symbol();
    return 0;
}

static int list_proc(object *params, object **result) {
    *result = params;
    return 0;
}

static int is_equal(object *a, object *b);
static int is_list_equal(object *this, object *that) {
    object *a, *b;

    while (!is_empty_list(this) && !is_empty_list(that)) {
        a = car(this);
        b = car(that);
        if (!is_equal(a, b)) {
            return 0;
        }
        this = cdr(this);
        that = cdr(that);
    }
    if (is_empty_list(this) && is_empty_list(that)) {
        return 1;
    }
    return 0;
}

static int is_eqv(object *a, object *b) {
    if (type(a) != type(b)) {
        return 0;
    }
    switch (type(a)) {
        case FIXNUM:
            return obj_nv(a) == obj_nv(b);
        case FLONUM:
            return obj_rv(a) == obj_rv(b);
        case CHARACTER:
            return obj_cv(a) == obj_cv(b);
        default:
            return a == b;
    }
    
    /* never here */
    return 0;
}

static int is_equal(object *a, object *b) {
    if (type(a) != type(b)) {
        return 0;
    }
    if (is_pair(a)) {
        return is_list_equal(a, b);
    } else {
        return is_eqv(a, b);
    }

    /* never here */
    return 0;
}

static int is_eq_proc(object *params, object **result) {
    object *a, *b;

    check_null(result);
    check_arg2(params);

    a = car(params);
    b = cadr(params);
    *result = (a == b) ? get_true_obj() : get_false_obj();
    return 0;
}

static int is_eqv_proc(object *params, object **result) {
    object *this, *that;

    check_null(result);
    check_arg2(params);

    this = car(params);
    that = cadr(params);
    if (is_eqv(this,that)) {
        *result = get_true_obj();
    } else {
        *result = get_false_obj();
    }
    return 0;
}

static int is_equal_proc(object *params, object **result) {
    object *a, *b;

    check_null(result);
    check_arg2(params);

    a = car(params);
    b = cadr(params);
    *result = is_equal(a, b) ? get_true_obj() : get_false_obj();
    return 0;
}

static int exit_proc(object *params, object **result) {
    if (result == NULL) {
        return SC_E_NULL;
    }
    if (!is_empty_list(params)) {
        return SC_E_ARITY;
    }
    *result = get_nrv_symbol();
    repl_exit();
    return 0;
}

static int apply_proc(object *params, object **result) {
    /* handled specially in sc_eval for tail call.
     *
     * this function exists so that apply can be treated
     * as normal function in Scheme code.
     */
    return SC_E_INV_STAT;
}

int is_apply(object *obj) {
    return is_primitive_proc(obj) &&
           obj_fv(obj) == apply_proc;
}

static int eval_proc(object *params, object **result) {
    /* handled specially in sc_eval for tail call.
     *
     * this function exists so that eval can be treated
     * as normal function in Scheme code.
     */
    return SC_E_INV_STAT;
}

int is_eval(object *obj) {
    return is_primitive_proc(obj) &&
           obj_fv(obj) == eval_proc;
}

static int interaction_environment_proc(object *params, object **result) {
    if (!is_empty_list(params)) {
        return SC_E_ARITY;
    }
    if (result == NULL) {
        return SC_E_NULL;
    }
    *result = get_repl_env();
    return 0;
}

static int null_environment_proc(object *params, object **result) {
    if (result == NULL) {
        return SC_E_NULL;
    }
    if (!is_empty_list(params)) {
        return SC_E_ARITY;
    }
    *result = make_null_env();
    return 0;
}

static int base_environment_proc(object *params, object **result) {
    if (result == NULL) {
        return SC_E_NULL;
    }
    if (!is_empty_list(params)) {
        return SC_E_ARITY;
    }
    *result = make_base_env();
    return 0;
}

static int gc_summary_proc(object *params, object **result) {
    if (result == NULL) {
        return SC_E_NULL;
    }
    if (!is_empty_list(params)) {
        return SC_E_ARITY;
    }
    dump_gc_summary();
    *result = get_nrv_symbol();
    return 0;
}

static int gc_proc(object *params, object **result) {
    if (result == NULL) {
        return SC_E_NULL;
    }
    if (!is_empty_list(params)) {
        return SC_E_ARITY;
    }
    gc();
    *result = get_nrv_symbol();
    return 0;
}

static int char_is_alpha_proc(object *params, object **result) {
    object *obj;

    check_null(result);
    check_arg1(params);

    obj = car(params);
    if (!is_character(obj)) {
        return SC_E_ARG_TYPE;
    }
    *result = (isalpha(obj_cv(obj))) ? get_true_obj() : get_false_obj();
    return 0;
}

static int char_is_num_proc(object *params, object **result) {
    object *obj;

    check_null(result);
    check_arg1(params);

    obj = car(params);
    if (!is_character(obj)) {
        return SC_E_ARG_TYPE;
    }
    *result = (isdigit(obj_cv(obj))) ? get_true_obj() : get_false_obj();
    return 0;
}

static int char_is_space_proc(object *params, object **result) {
    object *obj;

    check_null(result);
    check_arg1(params);
    obj = car(params);
    if (!is_character(obj)) {
        return SC_E_ARG_TYPE;
    }
    *result = (isspace(obj_cv(obj))) ? get_true_obj() : get_false_obj();
    return 0;
}

static int char_is_upper_proc(object *params, object **result) {
    object *obj;

    check_null(result);
    check_arg1(params);

    obj = car(params);
    if (!is_character(obj)) {
        return SC_E_ARG_TYPE;
    }

    *result = (isupper(obj_cv(obj))) ? get_true_obj() : get_false_obj();
    return 0;
}

static int char_is_lower_proc(object *params, object **result) {
    object *obj;

    check_null(result);
    check_arg1(params);

    obj = car(params);
    if (!is_character(obj)) {
        return SC_E_ARG_TYPE;
    }
    *result = (islower(obj_cv(obj))) ? get_true_obj() : get_false_obj();
    return 0;
}

static int char_upper_proc(object *params, object **result) {
    object *obj;
    int c;

    check_null(result);
    check_arg1(params);

    obj = car(params);
    if (!is_character(obj)) {
        return SC_E_ARG_TYPE;
    }
    c = toupper(obj_cv(obj));
    *result = make_character(c);
    if (*result == NULL) {
        return SC_E_NO_MEM;
    }
    return 0;
}

static int char_lower_proc(object *params, object **result) {
    object *obj;
    int c;

    check_null(result);
    check_arg1(params);

    obj = car(params);
    if (!is_character(obj)) {
        return SC_E_ARG_TYPE;
    }
    c = tolower(obj_cv(obj));
    *result = make_character(c);
    if (*result == NULL) {
        return SC_E_NO_MEM;
    }
    return 0;
}

#define DEFINE_LIST_PROC(name) \
    define_proc(#name, name ## _proc)

int init_primitive(object *env) {
    int ret;

    define_proc("null?", is_null_proc);
    define_proc("boolean?", is_boolean_proc);
    define_proc("symbol?", is_symbol_proc);
    define_proc("integer?", is_integer_proc);
    define_proc("real?", is_real_proc);
    define_proc("char?", is_char_proc);
    define_proc("string?", is_string_proc);
    define_proc("pair?", is_pair_proc);
    define_proc("procedure?", is_procedure_proc);

    define_proc("char->integer", char_to_integer_proc);
    define_proc("integer->char", integer_to_char_proc);
    define_proc("number->string", number_to_string_proc);
    define_proc("string->number", string_to_number_proc);
    define_proc("symbol->string", symbol_to_string_proc);
    define_proc("string->symbol", string_to_symbol_proc);
    define_proc("exact->inexact", exact_to_inexact_proc);
    define_proc("inexact->exact", inexact_to_exact_proc);

    define_proc("+", add_proc);
    define_proc("-", sub_proc);
    define_proc("*", mul_proc);
    define_proc("/", div_proc);
    define_proc("quotient", quotient_proc);
    define_proc("remainder", remainder_proc);
    define_proc("=", is_number_equal_proc);
    define_proc("<", is_number_less_proc);
    define_proc(">", is_number_greater_proc);
    define_proc("<=", is_number_less_equal_proc);
    define_proc(">=", is_number_greater_equal_proc);

    define_proc("char-alphabetic?", char_is_alpha_proc);
    define_proc("char-numeric?", char_is_num_proc);
    define_proc("char-whitespace?", char_is_space_proc);
    define_proc("char-upper-case?", char_is_upper_proc);
    define_proc("char-lower-case?", char_is_lower_proc);
    define_proc("char-upcase", char_upper_proc);
    define_proc("char-downcase", char_lower_proc);

    define_proc("cons", cons_proc);
    DEFINE_LIST_PROC(car);
    DEFINE_LIST_PROC(cdr);
    DEFINE_LIST_PROC(cdar);
    DEFINE_LIST_PROC(cadr);
    DEFINE_LIST_PROC(caar);
    DEFINE_LIST_PROC(cddr);
    DEFINE_LIST_PROC(caaar);
    DEFINE_LIST_PROC(caadr);
    DEFINE_LIST_PROC(cadar);
    DEFINE_LIST_PROC(caddr);
    DEFINE_LIST_PROC(cdaar);
    DEFINE_LIST_PROC(cdadr);
    DEFINE_LIST_PROC(cddar);
    DEFINE_LIST_PROC(cdddr);
    DEFINE_LIST_PROC(caaaar);
    DEFINE_LIST_PROC(caaadr);
    DEFINE_LIST_PROC(caadar);
    DEFINE_LIST_PROC(caaddr);
    DEFINE_LIST_PROC(cadaar);
    DEFINE_LIST_PROC(cadadr);
    DEFINE_LIST_PROC(caddar);
    DEFINE_LIST_PROC(cadddr);
    DEFINE_LIST_PROC(cdaaar);
    DEFINE_LIST_PROC(cdaadr);
    DEFINE_LIST_PROC(cdadar);
    DEFINE_LIST_PROC(cdaddr);
    DEFINE_LIST_PROC(cddaar);
    DEFINE_LIST_PROC(cddadr);
    DEFINE_LIST_PROC(cdddar);
    DEFINE_LIST_PROC(cddddr);
    define_proc("set-car!", set_car_proc);
    define_proc("set-cdr!", set_cdr_proc);
    define_proc("list", list_proc);

    define_proc("eqv?", is_eqv_proc);
    define_proc("eq?", is_eq_proc);
    define_proc("equal?", is_equal_proc);

    define_proc("exit", exit_proc);
    
    define_proc("interaction-environment", interaction_environment_proc);
    define_proc("null-environment", null_environment_proc);
    define_proc("base-environment", base_environment_proc);

    define_proc("eval", eval_proc);
    define_proc("apply", apply_proc);

    define_proc("gc", gc_proc);
    define_proc("gc-summary", gc_summary_proc);

    ret = init_io_primitive(env);
    if (ret != 0) {
        return ret;
    }
    ret = init_math_primitive(env);
    return ret;
}

