#include <math.h>
#include <stdlib.h>
#include "object.h"
#include "procdef.h"
#include "mathproc.h"

static int unary_fn(object *params,
                    object **result,
                    double (*fn)(double)) {
    object *obj;
    double ret, x;

    check_null(result);
    check_arg1(params);

    obj = car(params);
    if (!is_number(obj)) {
        return SC_E_ARG_TYPE;
    }
    x = number_to_double(obj);
    ret = fn(x);
    *result = make_flonum(ret);
    if (*result == NULL) {
        return SC_E_NO_MEM;
    }
    return 0;
}

#define MAKE_UNARY_PROC(name) \
    static int name ## _proc(object *params, object **result) { \
        return unary_fn(params, result, name); \
    }

MAKE_UNARY_PROC(sin);
MAKE_UNARY_PROC(cos);
MAKE_UNARY_PROC(tan);
MAKE_UNARY_PROC(asin);
MAKE_UNARY_PROC(acos);
MAKE_UNARY_PROC(atan);
MAKE_UNARY_PROC(sinh);
MAKE_UNARY_PROC(cosh);
MAKE_UNARY_PROC(tanh);
MAKE_UNARY_PROC(asinh);
MAKE_UNARY_PROC(acosh);
MAKE_UNARY_PROC(atanh);
MAKE_UNARY_PROC(exp);
MAKE_UNARY_PROC(log);
MAKE_UNARY_PROC(log10);
MAKE_UNARY_PROC(sqrt);
MAKE_UNARY_PROC(cbrt);
MAKE_UNARY_PROC(ceil);
MAKE_UNARY_PROC(floor);
MAKE_UNARY_PROC(trunc);
MAKE_UNARY_PROC(round);

static int pow_proc(object *params, object **result) {
    double x, y, ret;
    object *xobj, *yobj;

    check_null(result);
    check_arg2(params);
    
    xobj = car(params);
    yobj = cadr(params);
    if (!is_number(xobj) || !is_number(yobj)) {
        return SC_E_ARG_TYPE;
    }
    x = number_to_double(xobj);
    y = number_to_double(yobj);
    ret = pow(x, y);    /* domain error not handled */
    *result = make_flonum(ret);
    if (*result == NULL) {
        return SC_E_NO_MEM;
    }
    return 0;
}

static int abs_proc(object *params, object **result) {
    long n;
    double d;
    object *obj;

    check_null(result);
    check_arg1(params);
    obj = car(params);
    if (is_fixnum(obj)) {
        n = obj_nv(obj);
        n = labs(n);
        *result = make_fixnum(n);
    } else if (is_flonum(obj)) {
        d = obj_rv(obj);
        d = fabs(d);
        *result = make_flonum(d);
    } else {
        return SC_E_ARG_TYPE;
    }
    if (*result == NULL) {
        return SC_E_NO_MEM;
    }
    return 0;
}



int init_math_primitive(object *env) {
    define_proc("sin", sin_proc);
    define_proc("cos", cos_proc);
    define_proc("tan", tan_proc);
    define_proc("asin", asin_proc);
    define_proc("acos", acos_proc);
    define_proc("atan", atan_proc);
    define_proc("sinh", sinh_proc);
    define_proc("cosh", cosh_proc);
    define_proc("tanh", tanh_proc);
    define_proc("asinh", asinh_proc);
    define_proc("acosh", acosh_proc);
    define_proc("atanh", atanh_proc);
    define_proc("exp", exp_proc);
    define_proc("log", log_proc);
    define_proc("log10", log10_proc);
    define_proc("sqrt", sqrt_proc);
    define_proc("ceil", ceil_proc);
    define_proc("floor", floor_proc);
    define_proc("truncate", trunc_proc);
    define_proc("round", round_proc);
    define_proc("cbrt", cbrt_proc);
    define_proc("expt", pow_proc);
    define_proc("abs", abs_proc);

    return 0;
}

