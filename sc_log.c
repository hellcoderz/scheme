#include <stdio.h>
#include "sc_log.h"

void _sc_log(char *msg) {
    fprintf(stderr, "%s", msg);
}

