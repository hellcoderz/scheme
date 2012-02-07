#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "sc_object.h"
#include "sc_mem.h"
#include "sc_symtbl.h"
#include "sc_log.h"

typedef struct node {
    object *sym;
    unsigned int hash;
    struct node *next;
} node;

typedef struct bucket {
    node *next;
} bucket;

#define DEFAULT_BUCKET_NUM 4099


symtbl* symtbl_new() {
    symtbl *tbl;
    bucket *bucketp;

    tbl = sc_malloc(sizeof(symtbl));
    if (tbl == NULL) {
        sc_log("no memory for symtbl\n");
        return NULL;
    }

    bucketp = sc_malloc(sizeof(bucket) * DEFAULT_BUCKET_NUM);
    if (bucketp == NULL) {
        sc_log("no memory for symtbl\n");
        sc_free(tbl);
        return NULL;
    }
    memset(bucketp, 0, sizeof(bucket) * DEFAULT_BUCKET_NUM);
    tbl->bsize = DEFAULT_BUCKET_NUM;
    tbl->buckets = bucketp;
    return tbl;
}

void symtbl_dispose(symtbl *tbl) {
    /* TODO not implemented */
    return;
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

object* symtbl_insert(symtbl *tbl, char *sym) {
    unsigned int h;
    object *sym_obj;
    bucket *p;
    node *np;
    int i;

    h = hash(sym);
    i = h % tbl->bsize;
    p = (bucket*)(tbl->buckets) + i;
    np = p->next;
    while (np != NULL) {
        if (np->hash == h && 
            strcmp(sym, obj_iv(np->sym)) == 0) {
            break;
        }
        np = np->next;
    }

    if (np == NULL) {
        /* insert new */
        sym_obj = internal_make_symbol(sym);
        if (sym_obj == NULL) {
            return NULL;
        }
        np = sc_malloc(sizeof(node));
        if (np == NULL) {
            sc_log("no memory for symtbl\n");
            return NULL;
        }
        np->next = p->next;
        p->next = np;
        np->sym = sym_obj;
        np->hash = h;
    } else {
        sym_obj = np->sym;
    }
    return sym_obj;
}

