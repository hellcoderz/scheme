#include <stddef.h>
#include "sc_object.h"
#include "sc_sform.h"

static object *g_quote_symbol;

int sform_init() {
    object *obj;

    obj = make_symbol("quote");
    if (obj == NULL) {
        return -1;
    }

    g_quote_symbol = obj;
    return 0;
}

object* get_quote_symbol() {
    return g_quote_symbol;
}

