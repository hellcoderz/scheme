#include "sc_object.h"
#include "sc_mem.h"

object* make_fixnum(long value) {
    object *obj;

    obj = alloc_object();
    obj->type = FIXNUM;
    obj->data.fixnum.value = value;
    return obj;
}

int is_fixnum(object *obj) {
    return obj->type == FIXNUM;
}

