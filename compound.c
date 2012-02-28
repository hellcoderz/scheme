#include "object.h"
#include "mem.h"
#include "gc.h"

int is_compound_proc(object *obj) {
    return obj != NULL && type(obj) == COMPOUND_PROC;
}

/* make parameters a proper list */
static unsigned char normalize_params(object *params, object **result, char *has_vararg) {
    object *rest = params;
    object *prev = NULL;
    unsigned char argc = 0;

    *has_vararg = 0;
    *result = params;
    /* count # of parameters */
    while (!is_empty_list(rest)) {
        if (is_pair(rest)) {
            prev = rest;
            rest = cdr(rest);
            argc++;
        } else {
            /* var-arg */
            *has_vararg = 1;
            break;
        }
    }
    if (*has_vararg) {
        if (prev == NULL) {
            *result = cons(rest, get_empty_list());
        } else {
            set_cdr(prev, cons(rest, get_empty_list()));
        }
    }
    return argc;
}
object* make_compound_proc(object *params,
                           object *body,
                           object *env) {
    object *obj;
    object *norm_params;
    unsigned char argc;
    char has_vararg;
    
    argc = normalize_params(params, &norm_params, &has_vararg);
    gc_protect(norm_params);
    obj = alloc_object();
    gc_abandon();

    type(obj) = COMPOUND_PROC;
    obj_lvp(obj) = norm_params;
    obj_lvb(obj) = body;
    obj_lve(obj) = env;
    obj_lvargc(obj) = argc;
    obj_lvvar(obj) = has_vararg;
    return obj;
}

