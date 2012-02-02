#include <stdio.h>
#include "sc_repl.h"
#include "sc_config.h"

int main(int argc, char **argv) {
    printf("%s", WELCOME_STR);

    return sc_repl();
}

