#include <string.h>
#include "sc_object.h"
#include "sc_mem.h"

object* make_string(char *str) {
    object *obj;
    char *p;

    obj = alloc_object();
    p = strdup(str);
    obj_sv(obj) = p;
    type(obj) = STRING;
    return obj;
}

int is_string(object *obj) {
    return type(obj) == STRING;
}

