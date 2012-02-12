#include "sc_object.h"
#include "sc_mem.h"

object* make_primitive_proc(prim_proc fn) {
    object *obj;

    obj = alloc_object();
    type(obj) = PRIMITIVE_PROC;
    obj_fv(obj) = fn;
    return obj;
}

int is_primitive_proc(object *obj) {
    return obj != NULL && type(obj) == PRIMITIVE_PROC;
}

