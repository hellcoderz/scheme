#include "sc_object.h"
#include "sc_mem.h"

int is_compound_proc(object *obj) {
    return obj != NULL && type(obj) == COMPOUND_PROC;
}

object* make_compound_proc(object *params,
                           object *body,
                           object *env) {
    object *obj;

    obj = alloc_object();
    type(obj) = COMPOUND_PROC;
    obj_lvp(obj) = params;
    obj_lvb(obj) = body;
    obj_lve(obj) = env;
    return obj;
}

