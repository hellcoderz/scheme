#include <assert.h>
#include "intcache.h"
#include "mem.h"
#include "config.h"
#include "log.h"

#define CACHE_SIZE (INT_CACHE_HIGH - INT_CACHE_LOW + 1)

static object *int_cache;

int intcache_init(void) {
    int size = CACHE_SIZE * sizeof(object);
    long num;
    int i;
    object *obj;
    
    int_cache = sc_malloc(size);
    if (int_cache == NULL) {
        sc_log("%s", "failed to initialize integer cache");
        return -1;
    }
    
    for (i = 0, num = INT_CACHE_LOW; i != CACHE_SIZE; i++, num++) {
        obj = int_cache + i;
        type(obj) = FIXNUM;
        obj_nv(obj) = num;
    }
    return 0;
}

void intcache_dispose(void) {
    sc_free(int_cache);
    int_cache = NULL;
}

object* intcache_get(long val) {
    assert(int_cache != NULL);

    if (val >= INT_CACHE_LOW && val <= INT_CACHE_HIGH) {
        return int_cache + (val - INT_CACHE_LOW);
    }
    return NULL;
}

