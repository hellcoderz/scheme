#include "object.h"
#include "mem.h"

int is_flonum(object *obj) {
    return obj != NULL && type(obj) == FLONUM;
}

object* make_flonum(double val) {
    object *obj;

    obj = alloc_object();
    type(obj) = FLONUM;
    obj_rv(obj) = val;
    return obj;
}

