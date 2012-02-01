#include "sc_object"
#include "sc_mem.h"

object* make_fixnum(long value) {
    object *obj;

    obj = alloc_object();
    obj->type = FIXNUM;
    obj->data.fixnum.value = value;
    return obj;
}

bool is_fixnum(object *obj) {
    return obj->type == FIXNUM;
}

