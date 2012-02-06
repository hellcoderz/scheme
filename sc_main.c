#include <stdio.h>
#include "sc_config.h"
#include "sc_object.h"
#include "sc_repl.h"

int main(int argc, char **argv) {
    if (init_obj() != 0) {
       return -1;
    }

    printf("%s", WELCOME_STR);

    return sc_repl();
}

