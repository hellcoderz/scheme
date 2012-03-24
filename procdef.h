#ifndef _SC_PROCDEF_H_
#define _SC_PROCDEF_H_

#include "object.h"

enum {
    SC_E_ARG_TYPE = 100,
    SC_E_NO_MEM,
    SC_E_NULL,
    SC_E_ARITY,
    SC_E_ARG_INVL,
    SC_E_DIV,
    SC_E_INV_STAT,
    SC_E_IO_OPEN,
    SC_E_IO_CLOSE,
    SC_E_IO_READ,
    SC_E_IO_INVL_PORT,
    SC_E_LOAD,
    SC_E_INVL_INDEX,
};


#define check_null(p) \
    if (p == NULL) { \
        return SC_E_NULL; \
    }
#define check_arg2(p) \
    if (!is_empty_list(cddr(p))) { \
        return SC_E_ARITY; \
    }
#define check_arg1(p) \
    if (!is_empty_list(cdr(p))) { \
        return SC_E_ARITY; \
    }
#define check_arg3(p) \
    if (!is_empty_list(cdddr(p))) { \
        return SC_E_ARITY; \
    }
#define check_arg0(p) \
    if (!is_empty_list(p)) { \
        return SC_E_ARITY; \
    }

int init_primitive(object *env);
char* error_str(int err);
int is_apply(object *obj);
int is_eval(object *obj);
int is_callwcc(object *exp);
int env_define_proc(char *sym, prim_proc fn, object *env);

double number_to_double(object *obj);
int is_number(object *obj);

#define define_proc(x, y) \
    env_define_proc(x, y, env)

#endif

