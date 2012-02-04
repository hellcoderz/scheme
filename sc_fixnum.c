#include "sc_object.h"
#include "sc_mem.h"

object* make_fixnum(long value) {
    object *obj;

    obj = alloc_object();
    type(obj) = FIXNUM;
    obj_nv(obj) = value;
    return obj;
}

int is_fixnum(object *obj) {
    if (obj == NULL) {
        return 0;
    }

    return type(obj) == FIXNUM;
}

