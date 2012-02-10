#include <stdio.h>
#include "sc_object.h"
#include "sc_mem.h"
#include "sc_log.h"

#define INIT_BOOLEAN(p, v) \
    p = alloc_object(); \
    obj_bv(p) = v; \
    type(p) = BOOLEAN;

object *g_true_val;
object *g_false_val;

object* make_boolean(int value) {
    object *obj;

    if (value == 't') {
       obj = g_true_val; 
    } else if (value == 'f') {
        obj = g_false_val;
    } else {
        obj = NULL;
        char msg[64];
        sprintf(msg, "Invalid parameter in make_boolean(%c)\n", value);
        sc_log(msg);
    }

    return obj;
}

int is_boolean(object *obj) {
    if (obj == NULL) {
        return 0;
    }

    return type(obj) == BOOLEAN;
}

int is_true(object *obj) {
    return obj != g_false_val && obj != NULL;
}

int is_false(object *obj) {
    return obj == g_false_val;
}

int boolean_init(void) {
    INIT_BOOLEAN(g_true_val, 1);
    INIT_BOOLEAN(g_false_val, 0);

    return 0;
}

