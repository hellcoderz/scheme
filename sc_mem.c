#include <stdlib.h>
#include "sc_mem.h"
#include "sc_log.h"

void* sc_malloc(size_t size) {
    return malloc(size);
}

void sc_free(void *ptr) {
    free(ptr);
}

void* sc_realloc(void *ptr, size_t size) {
    return realloc(ptr, size);
}

object* alloc_object(void) {
    object *obj;

    obj = sc_malloc(sizeof(object));
    if (obj == NULL) {
        sc_log("out of memory");
        exit(1);
    }
    return obj;
}

