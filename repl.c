#include <stdio.h>
#include "config.h"
#include "object.h"
#include "reader.h"
#include "write.h"
#include "eval.h"
#include "repl.h"
#include "env.h"
#include "gc.h"

#define CORE_LIB_PATH APP_DIR "lib/core.scm"

static int keep_run = 1;
static object *global_env;

static int load_src(char *filename) {
    FILE *in;
    object *exp;

    in = fopen(filename, "r");
    if (in == NULL) {
        return -1;
    }
    for (;;) {
        exp = sc_read(in);
        if (exp == NULL) {
            fclose(in);
            return -1;
        }
        if (is_eof_object(exp)) {
            break;
        }

        gc_protect(exp);
        exp = sc_eval(exp, global_env);
        if (exp == NULL) {
            fclose(in);
            return -1;
        }
        gc_abandon();
    }
    fclose(in);
    return 0;
}

static int load_core_lib() {
    return load_src(CORE_LIB_PATH);
}

static int init(void) {
    global_env = make_base_env();
    if (global_env == NULL) {
        return -1;
    }

    if (load_core_lib() != 0) {
        fprintf(stderr, "failed to load core lib\n");
        return -1;
    }

    return 0;
}

int sc_repl(char *run_file) {
    object *exp, *val;
    int ret = 0;
    int err_cnt = 0;
    FILE *in, *out;

    if (init() != 0) {
        return -1;
    }

    /* execute program */
    if (run_file) {
        int ret = load_src(run_file);
        if (ret != 0) {
            fprintf(stderr, "failed to load %s\n", run_file);
        }
        return ret;
    }

    /* inteactive mode */
    in = stdin;
    out = stdout;
    printf("%s", WELCOME_STR);
    while (keep_run) {
        if (err_cnt > 0) {
            printf("%d%s", err_cnt, PROMPT);
        } else {
            printf("%s", PROMPT);
        }

        exp = sc_read(in);
        if (exp == NULL) {
            err_cnt++;
            continue;
        }
        if (is_eof_object(exp)) {
            break;
        }

        gc_protect(exp);
        val = sc_eval(exp, global_env);
        gc_abandon();
        if (val == NULL) {
            err_cnt++;
            continue;
        }

        ret = sc_write(out, val);
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

