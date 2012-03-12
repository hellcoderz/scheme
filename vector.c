#include "object.h"
#include "mem.h"

int is_vector(object *obj) {
    return obj != NULL && type(obj) == VECTOR;
}

object* make_vector(object **arr, int size) {
    object *p;

    p = alloc_object();
    type(p) = VECTOR;
    obj_vsv(p) = size;
    obj_vav(p) = arr;
    return p;
}

void vector_free(object *obj) {
    if (obj == NULL) {
        return;
    }

    sc_free(obj_vav(obj));
}
