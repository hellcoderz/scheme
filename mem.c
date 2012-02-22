#include <stdlib.h>
#include <stdio.h>
#include "mem.h"
#include "gc.h"

void* sc_malloc(size_t size) {
    return malloc(size);
}

void sc_free(void *ptr) {
    free(ptr);
}

void* sc_realloc(void *ptr, size_t size) {
    return realloc(ptr, size);
}

static void error(char *msg) {
    fprintf(stderr, "%s\n", msg);
    exit(-1);
}

object* alloc_object(void) {
    object *obj;

    obj = gc_alloc();
    if (obj == NULL) {
        error("not enough memory for object");
    }
    return obj;
}

