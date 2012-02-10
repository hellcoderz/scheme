#include <string.h>
#include "sc_object.h"
#include "sc_mem.h"

object *g_the_empty_list;

object* get_empty_list() {
    return g_the_empty_list;
}

int is_empty_list(object *obj) {
    return type(obj) == THE_EMPTY_LIST;
}

int empty_list_init() {
    object *p;

    p = alloc_object();
    memset(p, 0, sizeof(object));
    type(p) = THE_EMPTY_LIST;
    g_the_empty_list = p;
    return 0;
}


int is_pair(object *obj) {
    return type(obj) == PAIR;
}

object* cons(object *car, object *cdr) {
    object *p;

    p = alloc_object();
    type(p) = PAIR;
    obj_pv(p).car = car;
    obj_pv(p).cdr = cdr;
    return p;
}

object* car(object *pair) {
    if (pair == NULL || !is_pair(pair)) {
        return NULL;
    }

    return obj_pv(pair).car;
}

object* cdr(object *pair) {
    if (pair == NULL || !is_pair(pair)) {
        return NULL;
    }

    return obj_pv(pair).cdr;
}

int set_car(object *pair, object *car) {
    if (is_pair(pair)) {
        obj_pv(pair).car = car;
        return 0;
    }
    return -1;
}

int set_cdr(object *pair, object *cdr) {
    if (is_pair(pair)) {
        obj_pv(pair).cdr = cdr;
        return 0;
    }

    return -1;
}

