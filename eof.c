#include <string.h>
#include "object.h"
#include "mem.h"

static object *eof_obj;

object* get_eof_object(void) {
    return eof_obj;
}

int is_eof_object(object *obj) {
    return eof_obj == obj;
}

int eof_init(void) {
    object *obj;

    obj = alloc_object();
    memset(obj, 0, sizeof(object));
    type(obj) = EOF_OBJECT;
    eof_obj = obj;
    return 0;
}

