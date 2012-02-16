#include <stdio.h>
#include "log.h"

void _sc_log(char *msg) {
    fprintf(stderr, "[LOG]%s", msg);
}

