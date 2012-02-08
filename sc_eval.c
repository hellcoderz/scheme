#include <stdio.h>
#include "sc_eval.h"
#include "sc_object.h"
#include "sc_log.h"
#include "sc_sform.h"

static int is_self_evaluate(object *exp) {
    return is_fixnum(exp) ||
           is_boolean(exp) ||
           is_character(exp) ||
           is_string(exp);
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

object* sc_eval(object *exp) {
    object *val;

    if (exp == NULL) {
        sc_log("cannot eval NULL exp\n");
        return NULL;
    }

    if (is_self_evaluate(exp)) {
        val = exp;
    } else if (is_quoted(exp)) {
        val = eval_quote(exp);
    } else {
        val = NULL;
        fprintf(stderr,
                "cannot evaluate expression\n");
    }

    return val;
}

