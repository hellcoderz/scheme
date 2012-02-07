#ifndef _SC_SYMTBL_H_
#define _SC_SYMTBL_H_

typedef struct symtbl {
    void *buckets;
    int bsize;
} symtbl;

symtbl* symtbl_new();
void symtbl_dispose(symtbl *tbl);
object* symtbl_insert(symtbl *tbl, char *sym);

#endif

