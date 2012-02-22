#include <stdio.h>
#include "repl.h"
#include "object.h"
#include "gc.h"

static void dispose(void) {
    dispose_obj();
    gc_finalize();
}

static int init(void) {
    int ret;

    ret = gc_init(-1);
    if (ret != 0) {
        fprintf(stderr, "gc system failed to initialize\n");
        return ret;
    }

    ret = init_obj();
    if (ret != 0) {
        return ret;
    }
    return 0;
}

int main(int argc, char **argv) {
    int ret;

    ret = init();
    if (ret != 0) {
        fprintf(stderr, "%s\n",
                "failed to intialize object system");
        return ret;
    }

    ret = sc_repl();

    dispose();

    return ret;
}

