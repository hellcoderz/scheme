#include <stdlib.h>
#include <string.h>
#include "object.h"
#include "mem.h"
#include "hashtbl.h"
#include "log.h"

hashtbl *symbols;

static object* internal_make_symbol(char *sym) {
    object *sym_obj;
    char *p;
    int len;

    len = strlen(sym);
    p = sc_malloc(len + 1);
    if (p == NULL) {
        sc_log("%s", "no memory");
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

void symbol_free(object *obj) {
    char *sym;

    if (obj == NULL) {
        return;
    }

    sym = obj_iv(obj);
    hashtbl_remove(symbols, obj, sym);
    sc_free(sym);
}

