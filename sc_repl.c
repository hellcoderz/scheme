#include <stdio.h>
#include "sc_config.h"
#include "sc_object.h"
#include "sc_reader.h"
#include "sc_print.h"
#include "sc_eval.h"
#include "sc_repl.h"
#include "sc_env.h"

static int keep_run = 1;
static object *global_env;

static int init(void) {
    global_env = make_base_env();
    if (global_env == NULL) {
        return -1;
    }

    return 0;
}

int sc_repl(void) {
    object *exp, *val;
    int ret = 0;
    int err_cnt = 0;
    FILE *in;

    if (init() != 0) {
        return -1;
    }

    in = stdin;
    printf("%s", WELCOME_STR);
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

        val = sc_eval(exp, global_env);
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

object* get_repl_env(void) {
    return global_env;
}

