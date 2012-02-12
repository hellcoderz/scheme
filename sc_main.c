#include <stdio.h>
#include "sc_config.h"
#include "sc_object.h"
#include "sc_repl.h"
#include "sc_procdef.h"

static int init(void) {
    if (init_obj() != 0) {
       return -1;
    }
    if (init_primitive() != 0) {
        return -1;
    }

    return 0;
}

int main(int argc, char **argv) {
    if (init() != 0) {
        return -1;
    }

    printf("%s", WELCOME_STR);

    return sc_repl();
}

