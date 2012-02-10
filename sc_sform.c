#include <stddef.h>
#include "sc_object.h"
#include "sc_sform.h"

static object *g_quote_symbol;
static object *g_set_symbol;
static object *g_define_symbol;
static object *g_nrv_symbol; /* No Return Value symbol */
static object *g_if_symbol;

#define DEFINE_SYMBOL(var, sym) \
{                           \
    object *obj;            \
    obj = make_symbol(sym); \
    if (obj == NULL) {      \
        return -1;          \
    }                       \
    var = obj;              \
}

int sform_init(void) {
    DEFINE_SYMBOL(g_quote_symbol, "quote");
    DEFINE_SYMBOL(g_set_symbol, "set!");
    DEFINE_SYMBOL(g_define_symbol, "define");
    DEFINE_SYMBOL(g_nrv_symbol, "; no value returned");
    DEFINE_SYMBOL(g_if_symbol, "if");

    return 0;
}

object* get_quote_symbol(void) {
    return g_quote_symbol;
}

object* get_set_symbol(void) {
    return g_set_symbol;
}

object* get_define_symbol(void) {
    return g_define_symbol;
}

object* get_nrv_symbol(void) {
    return g_nrv_symbol;
}

object* get_if_symbol(void) {
    return g_if_symbol;
}

