#ifndef _SC_HASHTBL_H_
#define _SC_HASHTBL_H_

typedef object* (*create_fn)(char *sym);
typedef char* (*tostr_fn)(object *obj);

typedef struct hashtbl {
    void *buckets;
    int bsize;
    create_fn create;
    tostr_fn tostr;
} hashtbl;

hashtbl* hashtbl_new(create_fn create, tostr_fn tostr);
void hashtbl_dispose(hashtbl *tbl);
object* hashtbl_insert(hashtbl *tbl, char *sym);
void hashtbl_remove(hashtbl *tbl, object *obj, char *sym);

#endif

