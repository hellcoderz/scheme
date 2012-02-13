#include <stdlib.h>
#include <string.h>
#include "sc_object.h"
#include "sc_mem.h"
#include "sc_hashtbl.h"
#include "sc_log.h"

hashtbl *symbols;

static object* internal_make_symbol(char *sym) {
    object *sym_obj;
    char *p;
    int len;

    len = strlen(sym);
    p = sc_malloc(len + 1);
    if (p == NULL) {
        sc_log("no memory\n");
        return NULL;
    }
    strcpy(p, sym);

    sym_obj = alloc_object();
    obj_iv(sym_obj) = p;
    type(sym_obj) = SYMBOL;
    return sym_obj;
}

object* make_symbol(char *sym) {
    object *obj;

    obj = hashtbl_insert(symbols, sym);
    return obj;
}

int is_symbol(object *obj) {
    return obj != NULL && type(obj) == SYMBOL;
}

int symbol_init(void) {
    symbols = hashtbl_new(internal_make_symbol);
    if (symbols != NULL) {
        return 0;
    }
    return 1;
}

void symbol_dispose(void) {
    hashtbl_dispose(symbols);
}

