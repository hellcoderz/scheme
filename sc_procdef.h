#ifndef _SC_PROCDEF_H_
#define _SC_PROCDEF_H_

enum {
    SC_E_ARG_TYPE = 100,
    SC_E_NO_MEM,
    SC_E_NULL,
    SC_E_ARITY,
    SC_E_ARG_INVL,
    SC_E_DIV,
    SC_E_INV_STAT,
};

int init_primitive(void);
char* error_str(int err);
int is_apply(object *obj);

#endif

