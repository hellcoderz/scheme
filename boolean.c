#include <stdio.h>
#include "object.h"
#include "mem.h"
#include "log.h"

#define INIT_BOOLEAN(p, v) \
    p = sc_malloc(sizeof(object)); \
    if (p == NULL) { \
        return -1; \
    } \
    obj_bv(p) = v; \
    type(p) = BOOLEAN;

static object *g_true_val;
static object *g_false_val;

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

object* get_true_obj(void) {
    return g_true_val;
}

object* get_false_obj(void) {
    return g_false_val;
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

void boolean_dispose(void) {
    sc_free(g_false_val);
    sc_free(g_true_val);
}

