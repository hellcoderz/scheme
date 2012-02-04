#include <stdio.h>
#include "sc_object.h"
#include "sc_print.h"
#include "sc_log.h"

int sc_write(object *val) {
    int ret = 0;

    if (val == NULL) {
        sc_log("Cannot print NULL val\n");
        return -1;
    }

    if (is_fixnum(val)) {
        printf("%ld", val->data.fixnum.value);
    } else if (is_boolean(val)) {
        char v = is_true(val) ? 't' : 'f';
        printf("#%c", v);
    } else {
        fprintf(stderr,
                "unknown type, cannot print\n");
        ret = 1;
    }

    return ret;
}

