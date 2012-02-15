#ifndef _SC_PROCDEF_H_
#define _SC_PROCDEF_H_

#include "sc_object.h"

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
};

int init_primitive(object *env);
char* error_str(int err);
int is_apply(object *obj);
int is_eval(object *obj);
int env_define_proc(char *sym, prim_proc fn, object *env);

#endif

