#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "object.h"
#include "mem.h"
#include "hashtbl.h"
#include "log.h"
#include "config.h"

typedef struct node {
    object *sym;
    unsigned int hash;
    struct node *next;
} node;

typedef struct bucket {
    node *next;
} bucket;


hashtbl* hashtbl_new(create_fn create, tostr_fn tostr) {
    hashtbl *tbl;
    bucket *bucketp;

    if (create == NULL || tostr == NULL) {
        return NULL;
    }

    tbl = sc_malloc(sizeof(hashtbl));
    if (tbl == NULL) {
        sc_log("%s", "no memory for hashtbl");
        return NULL;
    }

    bucketp = sc_malloc(sizeof(bucket) * DEFAULT_BUCKET_NUM);
    if (bucketp == NULL) {
        sc_log("%s", "no memory for hashtbl");
        sc_free(tbl);
        return NULL;
    }
    memset(bucketp, 0, sizeof(bucket) * DEFAULT_BUCKET_NUM);
    tbl->bsize = DEFAULT_BUCKET_NUM;
    tbl->buckets = bucketp;
    tbl->create = create;
    tbl->tostr = tostr;
    return tbl;
}

void hashtbl_dispose(hashtbl *tbl) {
    bucket *bp;
    node *np, *next;
    int i;

    for (i = 0; i < tbl->bsize; i++) {
        bp = (bucket*)(tbl->buckets) + i;
        np = bp->next;
        while (np != NULL) {
            next = np->next;
            sc_free(np);
            np = next;
        }
    }
    sc_free(tbl->buckets);
    sc_free(tbl);
}

static unsigned int hash(char *data) {
    /* DJB hash, good hash function for string */
    char *p;
    unsigned int h = 5381;

    for (p = data; *p != '\0'; p++) {
        h = ((h << 5) + h) + *p;
    }
    return h;
}

void hashtbl_remove(hashtbl *tbl, object *obj, char *sym) {
    unsigned int h;
    int i;
    bucket *p;
    node *prev, *curr;

    if (tbl == NULL) {
        return;
    }

    h = hash(sym);
    i = h % tbl->bsize;
    p = (bucket*)(tbl->buckets) + i;
    curr = p->next;
    if (curr == NULL) {
        return;
    }

    /* special case: remove first node */
    if (curr->sym == obj) {
#ifdef DEBUG_HASHTBL
        fprintf(stderr, "remove node: %s\n", sym);
#endif
        p->next = curr->next;
        sc_free(curr);
    }
    prev = curr;
    curr = curr->next;
    while (curr != NULL) {
        if (curr->sym == obj) {
#ifdef DEBUG_HASHTBL
        fprintf(stderr, "remove node: %s\n", sym);
#endif
            prev->next = curr->next;
            sc_free(curr);
            break;
        }
        prev = curr;
        curr = curr->next;
    }
}

object* hashtbl_insert(hashtbl *tbl, char *sym) {
    unsigned int h;
    object *sym_obj;
    bucket *p;
    node *np;
    int i;

    if (tbl == NULL || tbl->create == NULL || tbl->tostr == NULL ) {
        return NULL;
    }

    h = hash(sym);
    i = h % tbl->bsize;
    p = (bucket*)(tbl->buckets) + i;
    np = p->next;

    while (np != NULL) {
        if (np->hash == h && 
            strcmp(sym, tbl->tostr(np->sym)) == 0) {
            break;
        }
        np = np->next;
    }

    if (np == NULL) {
        /* insert new */
#ifdef DEBUG_HASHTBL
        fprintf(stderr, "insert new node: %s\n", sym);
#endif

        sym_obj = tbl->create(sym);
        if (sym_obj == NULL) {
            return NULL;
        }
        np = sc_malloc(sizeof(node));
        if (np == NULL) {
            sc_log("%s", "no memory for hashtbl");
            exit(1);
        }
        np->next = p->next;
        p->next = np;
        np->sym = sym_obj;
        np->hash = h;
    } else {
#ifdef DEBUG_HASHTBL
        fprintf(stderr, "node already exists: %s\n", sym);
#endif
        sym_obj = np->sym;
    }
    return sym_obj;
}

