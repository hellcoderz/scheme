#include <stdio.h>
#include "sc_object.h"
#include "sc_print.h"

int sc_write(object *val) {
    int ret = 0;

    switch (val->type) {
        case FIXNUM:
            printf("%ld", val->data.fixnum.value);
            break;
        default:
            fprintf(stderr,
                    "unknown type, cannot print\n");
            ret = 1;
            break;
    }

    return ret;
}

