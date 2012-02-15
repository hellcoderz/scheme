#include <stddef.h>
#include "sc_object.h"
#include "sc_env.h"
#include "sc_procdef.h"


static object* first_frame(object *env) {
    return car(env);
}

static object* make_frame(object *vars, object *vals) {
    return cons(vars, vals);
}

static object* frame_vars(object *frame) {
    return car(frame);
}

static object* frame_vals(object *frame) {
    return cdr(frame);
}

static int add_binding_to_frame(object *var, object *val, object *frame) {
    set_car(frame, cons(var, car(frame)));
    set_cdr(frame, cons(val, cdr(frame)));
    return 0;
}

static object* enclosing_env(object *env) {
    return cdr(env);
}

object* extend_env(object *vars, object *vals, object *base_env) {
    return cons(make_frame(vars, vals), base_env);
}

static object* lookup_variable(object *var, object *env) {
    object *frame, *vars, *vals;

    while (!is_empty_list(env)) {
        frame = first_frame(env);
        vars = frame_vars(frame);
        vals = frame_vals(frame);
        while (!is_empty_list(vars)) {
            if (var == car(vars)) {
                return vals;
            }
            vars = cdr(vars);
            vals = cdr(vals);
        }
        env = enclosing_env(env);
    }
    return NULL;
}

int set_variable_val(object *var, object *val, object *env) {
    object *vals;

    vals = lookup_variable(var, env);
    if (vals == NULL) {
        return -1;
    }
    return set_car(vals, val);
}

object* lookup_variable_val(object *var, object *env) {
    object *vals;

    vals = lookup_variable(var, env);
    return car(vals);
}

int define_variable(object *var, object *val, object *env) {
    object *frame, *vars, *vals;

    frame = first_frame(env);
    vars = frame_vars(frame);
    vals = frame_vals(frame);
    while (!is_empty_list(vars)) {
        if (var == car(vars)) {
            set_car(vals, val);
            return 0;
        }
        vars = cdr(vars);
        vals = cdr(vals);
    }
    return add_binding_to_frame(var, val, frame);
}

object* make_null_env(void) {
    object *e;
    object *env;

    e = get_empty_list();
    env = extend_env(e, e, e);
    return env;
}

object* make_base_env(void) {
    object *env;

    env = make_null_env();
    init_primitive(env);
    return env;
}

int is_valid_env(object *env) {
    object *frame;
    object *vars, *vals;

    if (!is_pair(env)) {
        return 0;
    }
    frame = first_frame(env);
    if (!is_pair(frame)) {
        return 0;
    }

    vars = frame_vars(frame);
    if (!is_empty_list(vars) && !is_pair(vars)) {
        return 0;
    }
    vals = frame_vals(frame);
    if (!is_empty_list(vals) && !is_pair(vals)) {
        return 0;
    }
    return 1;
}

