#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "object.h"
#include "mem.h"
#include "hashtbl.h"
#include "log.h"

#define mark_orphan(obj) \
    (obj_ssv(obj) = ORPHAN_STR)
#define mark_intern(obj) \
    (obj_ssv(obj) = INTERNED_STR)

static hashtbl* g_strtbl;

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
    obj_ssv(obj) = INTERNED_STR;
    return obj;
}

static char* internal_tostr(object *obj) {
    if (!is_string(obj)) {
        return NULL;
    }
    return obj_sv(obj);
}

int string_init(void) {
    g_strtbl = hashtbl_new(internal_make_string, internal_tostr);
    if (g_strtbl == NULL) {
        sc_log("%s", "failed to initialize string table");
        return 1;
    }
    return 0;
}

void string_dispose(void) {
    hashtbl_dispose(g_strtbl);
}

object* make_string(char *str) {
    return hashtbl_insert(g_strtbl, str);
}

int is_string(object *obj) {
    return obj != NULL && type(obj) == STRING;
}

void string_free(object *obj) {
    char *str;

    str = string_remove(obj);
    sc_free(str);
}

char* string_remove(object *obj) {
    char *str;
    
    if (obj == NULL) {
        return NULL;
    }

    str = obj_sv(obj);
    hashtbl_remove(g_strtbl, obj, str);
    return str;
}

object* string_insert(object *obj) {
    int exists;

    if (!is_string(obj)) {
        return NULL;
    }
    exists = hashtbl_insert_obj(g_strtbl, obj_sv(obj), obj);
    if (exists == 1) {
        mark_orphan(obj);
    } else if (exists == 0) {
        mark_intern(obj);
    } else {
        fprintf(stderr, "invalid string table state\n");
        exit(1);
    }
    return obj;
}

