#include <stdio.h>
#include <ctype.h>
#include "sc_object.h"
#include "sc_print.h"
#include "sc_log.h"

static int write_character(object *val) {
    char v = obj_cv(val);
    
    printf("%s", "#\\");
    switch (v) {
        case NUL:
            printf("%s", "nul");
            break;

        case TAB:
            printf("%s", "tab");
            break;

        /* LINEFEED is equal to NEWLINE */
        case NEWLINE:
            printf("%s", "newline");
            break;

        case RETURN:
            printf("%s", "return");
            break;

        case SPACE:
            printf("%s", "space");
            break;

        default:
            printf("%c", v);
            if (!isgraph(v)) {
                char msg[64];
                sprintf(msg, "character not printable `%d\n", v);
                sc_log(msg);
            }
            break;
    }
    return 0;
}

int sc_write(object *val) {
    int ret = 0;

    if (val == NULL) {
        sc_log("Cannot print NULL val\n");
        return -1;
    }

    if (is_fixnum(val)) {
        printf("%ld", obj_nv(val));
    } else if (is_boolean(val)) {
        char v = is_true(val) ? 't' : 'f';
        printf("#%c", v);
    } else if (is_character(val)) {
        ret = write_character(val);
    } else {
        fprintf(stderr,
                "unknown type, cannot print\n");
        ret = 1;
    }

    return ret;
}

