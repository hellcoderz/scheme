#include "sc_object.h"
#include "sc_mem.h"

object* make_character(int val) {
    object *obj;

    obj = alloc_object();
    type(obj) = CHARACTER;
    obj_cv(obj) = val;

    return obj;
}

int is_character(object *obj) {
    return type(obj) == CHARACTER;
}

