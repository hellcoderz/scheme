#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "object.h"
#include "mem.h"
#include "log.h"

static object* internal_make_string(char *str) {
    object *obj;
    char *p;
    int len;
    
    if (str == NULL) {
        return NULL;
    }

    obj = alloc_object();
    /* do not use strdup, for a consistent way to do gc */
    len = strlen(str);
    p = sc_malloc(len + 1);
    if (p == NULL) {
        sc_log("%s", "no memory for string");
        exit(1);
    }
    strcpy(p, str);
    obj_sv(obj) = p;
    obj_slenv(obj) = len;
    type(obj) = STRING;
    return obj;
}

object* make_string(char *str) {
    return internal_make_string(str);
}

int is_string(object *obj) {
    return obj != NULL && type(obj) == STRING;
}

void string_free(object *obj) {
    char *str;

    if (!is_string(obj)) {
        str = obj_sv(obj);
        sc_free(str);
    }
}

