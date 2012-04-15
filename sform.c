#include <stddef.h>
#include "object.h"
#include "sform.h"

static object *g_quote_symbol;
static object *g_set_symbol;
static object *g_define_symbol;
static object *g_nrv_symbol; /* No Return Value symbol */
static object *g_if_symbol;
static object *g_lambda_symbol;
static object *g_begin_symbol;
static object *g_cond_symbol;
static object *g_else_symbol;
static object *g_let_symbol;
static object *g_and_symbol;
static object *g_or_symbol;
static object *g_callwcc_symbol;
static object *g_definemacro_symbol;

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
    DEFINE_SYMBOL(g_lambda_symbol, "lambda");
    DEFINE_SYMBOL(g_begin_symbol, "begin");
    DEFINE_SYMBOL(g_cond_symbol, "cond");
    DEFINE_SYMBOL(g_else_symbol, "else");
    DEFINE_SYMBOL(g_let_symbol, "let");
    DEFINE_SYMBOL(g_and_symbol, "and");
    DEFINE_SYMBOL(g_or_symbol, "or");
    DEFINE_SYMBOL(g_callwcc_symbol, "call-with-current-continuation");
    DEFINE_SYMBOL(g_definemacro_symbol, "define-macro");

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

object* get_lambda_symbol(void) {
    return g_lambda_symbol;
}

object* get_begin_symbol(void) {
    return g_begin_symbol;
}

object* get_cond_symbol(void) {
    return g_cond_symbol;
}

object* get_else_symbol(void) {
    return g_else_symbol;
}

object* get_let_symbol(void) {
    return g_let_symbol;
}

object* get_and_symbol(void) {
    return g_and_symbol;
}

object* get_or_symbol(void) {
    return g_or_symbol;
}

object* get_callwcc_symbol(void) {
    return g_callwcc_symbol;
}

object* get_definemacro_symbol(void) {
    return g_definemacro_symbol;
}

