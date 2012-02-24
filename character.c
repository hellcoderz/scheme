#include "object.h"
#include "mem.h"
#include "charcache.h"

object* make_character(int val) {
    object *obj;

    obj = charcache_get(val);
    if (obj != NULL) {
        return obj;
    }
    obj = alloc_object();
    type(obj) = CHARACTER;
    obj_cv(obj) = val;

    return obj;
}

int is_character(object *obj) {
    return obj != NULL && type(obj) == CHARACTER;
}

