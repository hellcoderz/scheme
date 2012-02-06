#include <string.h>
#include <stdlib.h>
#include "sc_object.h"
#include "sc_mem.h"
#include "sc_log.h"

object* make_string(char *str) {
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
        sc_log("no memory for string\n");
        exit(1);
    }
    strcpy(p, str);
    obj_sv(obj) = p;
    type(obj) = STRING;
    return obj;
}

int is_string(object *obj) {
    return type(obj) == STRING;
}

