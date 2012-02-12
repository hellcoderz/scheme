#include <stddef.h>
#include "sc_object.h"
#include "sc_procdef.h"
#include "sc_env.h"

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
    }
    return NULL;
}

static int add_proc(object *params, object **result) {
    long sum = 0;
    object *list = params;
    object *elem;
    
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

int init_primitive(void) {
    define_proc("+", add_proc);

    return 0;
}

