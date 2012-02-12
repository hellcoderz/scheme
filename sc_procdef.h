#ifndef _SC_PROCDEF_H_
#define _SC_PROCDEF_H_

enum {
    SC_E_ARG_TYPE = 100,
    SC_E_NO_MEM,
};

int init_primitive(void);
char* error_str(int err);

#endif

