#include <stdio.h>
#include "sc_eval.h"
#include "sc_object.h"
#include "sc_log.h"

object* sc_eval(object *exp) {
    object *val;

    if (exp == NULL) {
        sc_log("Cannot eval NULL exp\n");
        return NULL;
    }

    if (is_fixnum(exp)) {
        val = exp;
    } else if (is_boolean(exp)) {
        val = exp;
    } else if (is_character(exp)) {
        val = exp;
    } else {
        val = NULL;
        fprintf(stderr,
                "bad expression\n");
    }

    return val;
}

