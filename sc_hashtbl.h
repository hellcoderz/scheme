#ifndef _SC_HASHTBL_H_
#define _SC_HASHTBL_H_

typedef object* (*create_fn)(char *sym);

typedef struct hashtbl {
    void *buckets;
    int bsize;
    create_fn create;
} hashtbl;

hashtbl* hashtbl_new(create_fn fn);
void hashtbl_dispose(hashtbl *tbl);
object* hashtbl_insert(hashtbl *tbl, char *sym);

#endif

