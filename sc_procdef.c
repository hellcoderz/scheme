#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include "sc_object.h"
#include "sc_procdef.h"
#include "sc_env.h"
#include "sc_sform.h"
#include "sc_repl.h"

static int define_proc(char *sym, prim_proc fn) {
    object *sym_obj, *proc_obj;

    sym_obj = make_symbol(sym);
    proc_obj = make_primitive_proc(fn);
    return define_variable(sym_obj, proc_obj, get_global_env());
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

    if (result == NULL) {
        return SC_E_NULL;
    }
    if (!is_empty_list(cdr(params))) {
        return SC_E_ARITY;
    }
    if (!is_fixnum(car(params))) {
        return SC_E_ARG_TYPE;
    }

    sprintf(buf, "%ld", obj_nv(car(params)));
    *result = make_string(buf);
    if (*result == NULL) {
        return SC_E_NO_MEM;
    }
    return 0;
}

static int string_to_number_proc(object *params, object **result) {
    long lv;
    char *str;

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
    if (*str != '\0') {
        return SC_E_ARG_INVL;
    }

    lv = atol(obj_sv(car(params)));
    *result = make_fixnum(lv);
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

static int add_proc(object *params, object **result) {
    long sum = 0;
    object *list = params;
    object *elem;

    if (result == NULL) {
        return SC_E_NULL;
    }
    
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
    if (*result == NULL) {
        return SC_E_NO_MEM;
    }
    return 0;
}

static int sub_proc(object *params, object **result) {
    long remain;
    object *list;
    object *elem;

    if (result == NULL) {
        return SC_E_NULL;
    }
    if (is_empty_list(params)) {
        return SC_E_ARITY;
    }
   
    elem = car(params);
    if (!is_fixnum(elem)) {
        return SC_E_ARG_TYPE;
    }
    list = cdr(params);
    remain = obj_nv(elem);

    if (is_empty_list(cdr(params))) {
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
    if (*result == NULL) {
        return SC_E_NO_MEM;
    }
    return 0;
}

static int mul_proc(object *params, object **result) {
    long product = 1;
    object *list = params;
    object *elem;

    if (result == NULL) {
        return SC_E_NULL;
    }
    
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
    long val;
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
    if (!is_fixnum(elem)) {
        return SC_E_ARG_TYPE;
    }
    val = obj_nv(elem);
    false_obj = get_false_obj();
    while (!is_empty_list(list)) {
        elem = car(list);
        if (!is_fixnum(elem)) {
            return SC_E_ARG_TYPE;
        }
        switch (mode) {
            case EQ:
                if (val != obj_nv(elem)) {
                    *result = false_obj;
                    return 0;
                }
                break;
            case LESS:
                if (val >= obj_nv(elem)) {
                    *result = false_obj;
                    return 0;
                }
                break;
            case GREATER:
                if (val <= obj_nv(elem)) {
                    *result = false_obj;
                    return 0;
                }
                break;
            case LESS_EQ:
                if (val > obj_nv(elem)) {
                    *result = false_obj;
                    return 0;
                }
                break;
            case GREATER_EQ:
                if (val < obj_nv(elem)) {
                    *result = false_obj;
                    return 0;
                }
                break;
            default:
                *result = false_obj;
                return 0;
        }
        val = obj_nv(elem);
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

static int is_eq(object *a, object *b);
static int is_list_eq(object *this, object *that) {
    object *a, *b;

    while (!is_empty_list(this) && !is_empty_list(that)) {
        a = car(this);
        b = car(that);
        if (!is_eq(a, b)) {
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

static int is_eq(object *this, object *that) {
    object *a = this;
    object *b = that;

    if (type(a) != type(b)) {
        return 0;
    }
    switch (type(a)) {
        case FIXNUM:
            return obj_nv(a) == obj_nv(b);
        case CHARACTER:
            return obj_cv(a) == obj_cv(b);
        case PAIR:
            return is_list_eq(a, b);
        default:
            return a == b;
    }
    
    /* never here */
    return 0;
}

static int is_eq_proc(object *params, object **result) {
    object *this, *that;

    if (result == NULL) {
        return SC_E_NULL;
    }
    if (!is_empty_list(cddr(params))) {
        return SC_E_ARITY;
    }

    this = car(params);
    that = cadr(params);
    if (is_eq(this,that)) {
        *result = get_true_obj();
    } else {
        *result = get_false_obj();
    }
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

#define DEFINE_LIST_PROC(name) \
    define_proc(#name, name ## _proc)

int init_primitive(void) {
    define_proc("null?", is_null_proc);
    define_proc("boolean?", is_boolean_proc);
    define_proc("symbol?", is_symbol_proc);
    define_proc("integer?", is_integer_proc);
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

    define_proc("+", add_proc);
    define_proc("-", sub_proc);
    define_proc("*", mul_proc);
    define_proc("quotient", quotient_proc);
    define_proc("remainder", remainder_proc);
    define_proc("=", is_number_equal_proc);
    define_proc("<", is_number_less_proc);
    define_proc(">", is_number_greater_proc);
    define_proc("<=", is_number_less_equal_proc);
    define_proc(">=", is_number_greater_equal_proc);

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

    define_proc("eq?", is_eq_proc);

    define_proc("exit", exit_proc);
    define_proc("apply", apply_proc);
    return 0;
}

