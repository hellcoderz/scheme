#include "object.h"
#include "mem.h"

object* make_fixnum(long value) {
    object *obj;

    obj = alloc_object();
    type(obj) = FIXNUM;
    obj_nv(obj) = value;
    return obj;
}

int is_fixnum(object *obj) {
    return obj != NULL && type(obj) == FIXNUM;
}

