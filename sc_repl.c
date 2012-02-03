#include <stdio.h>
#include "sc_config.h"
#include "sc_object.h"
#include "sc_reader.h"
#include "sc_print.h"
#include "sc_eval.h"
#include "sc_repl.h"

int sc_repl(void) {
    object *exp, *val;
    int ret = 0;
    FILE *in;

    in = stdin;
    for (;;) {
        printf("%s", PROMPT);

        exp = sc_read(in);
        if (exp == NULL) {
            if (!feof(in)) {
                ret = 1;
            }
            break;
        }

        val = sc_eval(exp);
        if (val == NULL) {
            ret = 1;
            break;
        }

        ret = sc_write(val);
        printf("\n");
        if (ret != 0) {
            break;
        }
    }

    return ret;
}

