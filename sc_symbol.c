#include "sc_object.h"
#include "sc_mem.h"
#include "sc_symtbl.h"

symtbl *symbols;

object* make_symbol(char *sym) {
    object *obj;

    obj = symtbl_insert(symbols, sym);
    return obj;
}

int is_symbol(object *obj) {
    return obj != NULL && type(obj) == SYMBOL;
}

int symbol_init() {
    symbols = symtbl_new();
    if (symbols != NULL) {
        return 0;
    }
    return 1;
}

