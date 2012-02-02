#include <stdio.h>
#include "sc_eval.h"
#include "sc_object.h"

object* sc_eval(object *exp) {
    object *val;

    if (is_fixnum(exp)) {
        val = exp;
    } else {
        val = NULL;
        fprintf(stderr,
                "bad expression\n");
    }

    return val;
}

