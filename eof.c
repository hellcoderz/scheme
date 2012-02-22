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

    obj = sc_malloc(sizeof(object));
    if (obj == NULL) {
        return -1;
    }
    memset(obj, 0, sizeof(object));
    type(obj) = EOF_OBJECT;
    eof_obj = obj;
    return 0;
}

void eof_dispose(void) {
    sc_free(eof_obj);
}

