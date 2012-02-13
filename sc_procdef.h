#ifndef _SC_PROCDEF_H_
#define _SC_PROCDEF_H_

enum {
    SC_E_ARG_TYPE = 100,
    SC_E_NO_MEM,
    SC_E_NULL,
    SC_E_ARITY,
    SC_E_ARG_INVL,
};

int init_primitive(void);
char* error_str(int err);

#endif

