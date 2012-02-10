#include <stdio.h>
#include "sc_eval.h"
#include "sc_object.h"
#include "sc_log.h"
#include "sc_sform.h"
#include "sc_env.h"

static int is_self_evaluate(object *exp) {
    return is_fixnum(exp) ||
           is_boolean(exp) ||
           is_character(exp) ||
           is_string(exp);
}

static int is_variable(object *exp) {
    return is_symbol(exp);
}

static int is_tagged_list(object *exp, object *tag) {
    object *car_obj;

    if (is_pair(exp)) {
        car_obj = car(exp);
        return is_symbol(car_obj) && car_obj == tag;
    }
    return 0;
}

static int is_quoted(object *exp) {
    return is_tagged_list(exp, get_quote_symbol());
}

static object* eval_quote(object *exp) {
    return cadr(exp);
}

static int is_assignment(object *exp) {
    return is_tagged_list(exp, get_set_symbol());
}

static object* assignment_variable(object *exp) {
    return cadr(exp);
}

static object* assignment_value(object *exp) {
    return caddr(exp);
}

static object* eval_assignment(object *exp, object *env) {
    object *var, *val;
    char msg[] = "wrong arity\n";
    int ret;

    var = assignment_variable(exp);
    val = assignment_value(exp);
    if (val == NULL || var == NULL || 
        cadddr(exp) != NULL) {
        fprintf(stderr, "%s", msg);
        return NULL;
    }
    if (!is_variable(var)) {
        fprintf(stderr, "%s\n", "variable must be symbol");
        return NULL;
    }

    val = sc_eval(val, env);
    if (val == NULL) {
        return NULL;
    }
    ret = set_variable_val(var, val, env);
    if (ret != 0) {
        fprintf(stderr, "%s\n", "unbound variable");
        return NULL;
    }
    return get_nrv_symbol();
}

static int is_definition(object *exp) {
    return is_tagged_list(exp, get_define_symbol());
}

static object* definition_variable(object *exp) {
    return cadr(exp);
}

static object* definition_value(object *exp) {
    return caddr(exp);
}

static object* eval_definition(object *exp, object *env) {
    object *var, *val;
    char msg[] = "wrong arity\n";
    int ret;

    var = definition_variable(exp);
    val = definition_value(exp);
    if (val == NULL || var == NULL || 
        cadddr(exp) != NULL) {
        fprintf(stderr, "%s", msg);
        return NULL;
    }
    if (!is_variable(var)) {
        fprintf(stderr, "%s\n", "variable must be symbol");
        return NULL;
    }

    val = sc_eval(val, env);
    if (val == NULL) {
        return NULL;
    }
    ret = define_variable(var, val, env);
    if (ret != 0) {
        fprintf(stderr, "%s\n",
                "unexpected error, cannot define variable");
        return NULL;
    }
    return get_nrv_symbol();
}

static object* eval_variable(object *exp, object *env) {
    object *obj;

    obj = lookup_variable_val(exp, env);
    if (obj == NULL) {
        fprintf(stderr, "%s\n",
                "unbound variable");
    }
    return obj;
}

object* sc_eval(object *exp, object *env) {
    object *val;

    if (exp == NULL) {
        sc_log("cannot eval NULL exp\n");
        return NULL;
    }

    if (is_self_evaluate(exp)) {
        val = exp;
    } else if (is_variable(exp)) {
        val = eval_variable(exp, env);
    } else if (is_quoted(exp)) {
        val = eval_quote(exp);
    } else if (is_assignment(exp)) {
        val = eval_assignment(exp, env);
    } else if (is_definition(exp)) {
        val = eval_definition(exp, env);
    } else {
        val = NULL;
        fprintf(stderr,
                "cannot evaluate expression\n");
    }

    return val;
}

