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
    int err_cnt = 0;
    FILE *in;

    in = stdin;
    for (;;) {
        if (err_cnt > 0) {
            printf("%d%s", err_cnt, PROMPT);
        } else {
            printf("%s", PROMPT);
        }

        exp = sc_read(in);
        if (exp == NULL) {
            if (feof(in)) {
                break;
            }

            err_cnt++;
            continue;
        }

        val = sc_eval(exp);
        if (val == NULL) {
            err_cnt++;
            continue;
        }

        ret = sc_write(val);
        printf("\n");
        if (ret != 0) {
            break;
        }
    }

    return ret;
}

