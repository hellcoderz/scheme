#include <stddef.h>
#include "sc_object.h"
#include "sc_env.h"

static object *g_empty_env, *g_global_env;

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

int setup_env(void) {
    object *e;

    e = get_empty_list();
    g_empty_env = e;
    g_global_env = extend_env(e, e, g_empty_env);
    return 0;
}

object* get_global_env(void) {
    return g_global_env;
}

