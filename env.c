#include <stddef.h>
#include "object.h"
#include "env.h"
#include "procdef.h"
#include "gc.h"


static object* first_frame(object *env) {
    return car(env);
}

static object* enclosing_env(object *env) {
    return cdr(env);
}

static object* make_frame(object *vars, object *vals) {
    object *frame, *var, *val;
    int ret;

    frame = make_env_frame();
    if (frame == NULL) {
        return NULL;
    }
    while (is_pair(vars) && is_pair(vals)) {
        var = car(vars);
        val = car(vals);
        ret = env_frame_insert(frame, var, val);
        if (ret != 0) {
            return NULL;
        }
        vars = cdr(vars);
        vals = cdr(vals);
    }
    return frame;
}

object* extend_env(object *vars, object *vals, object *base_env) {
    object *obj;

    obj = make_frame(vars, vals);
    gc_protect(obj);
    obj = cons(obj, base_env);
    gc_abandon();

    return obj;
}

int set_variable_val(object *var, object *val, object *env) {
    object *frame;

    while (!is_empty_list(env)) {
        frame = first_frame(env);
        if (env_frame_change(frame, var, val) == 0) {
            return 0;
        }
        env = enclosing_env(env);
    }
    return -1;
}

object* lookup_variable_val(object *var, object *env) {
    object *frame, *val;

    while (!is_empty_list(env)) {
        frame = first_frame(env);
        val = env_frame_find(frame, var);
        if (val != NULL) {
            return val;
        }
        env = enclosing_env(env);
    }
    return NULL;
}

int define_variable(object *var, object *val, object *env) {
    object *frame;

    frame = first_frame(env);
    return env_frame_insert(frame, var, val);
}

object* make_null_env(void) {
    object *frame, *e;

    frame = make_env_frame();
    if (frame == NULL) {
        return NULL;
    }
    e = get_empty_list();
    gc_protect(frame);
    e = cons(frame, e);
    gc_abandon();
    return e;
}

object* make_base_env(void) {
    object *env;

    env = make_null_env();
    gc_protect(env);
    init_primitive(env);
    gc_abandon();
    return env;
}

int is_valid_env(object *env) {
    object *frame;

    while (is_pair(env)) {
        frame = first_frame(env);
        if (!is_env_frame(frame)) {
            return 0;
        }
        env = enclosing_env(env);
    }

    return 1;
}

