#include "object.h"
#include "mem.h"

int is_macro(object *obj) {
    return obj != NULL && type(obj) == MACRO;
}

object *make_macro(object *t) {
    object *obj = alloc_object();
    type(obj) = MACRO;
    obj_mv(obj) = t;
    return obj;
}

