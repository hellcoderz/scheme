#include <stdio.h>
#include "sc_repl.h"
#include "sc_object.h"

static void dispose(void) {
    dispose_obj();
}

static int init(void) {
    int ret;

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

