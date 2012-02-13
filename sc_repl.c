#include <stdio.h>
#include "sc_config.h"
#include "sc_object.h"
#include "sc_reader.h"
#include "sc_print.h"
#include "sc_eval.h"
#include "sc_repl.h"
#include "sc_env.h"

static int keep_run = 1;

int sc_repl(void) {
    object *exp, *val;
    int ret = 0;
    int err_cnt = 0;
    FILE *in;

    in = stdin;
    while (keep_run) {
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

        val = sc_eval(exp, get_global_env());
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

void repl_exit(void) {
    keep_run = 0;
}

