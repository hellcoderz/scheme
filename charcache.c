#include "charcache.h"
#include "mem.h"
#include "log.h"

#define CACHE_START ' '
#define CACHE_END '~'
#define CACHE_SIZE (CACHE_END - CACHE_START + 1)

static object *char_cache;

int charcache_init(void) {
    int size = CACHE_SIZE * sizeof(object);
    int i;
    object *obj;

    char_cache = sc_malloc(size);
    if (char_cache == NULL) {
        sc_log("failed to initialize character cache\n");
        return -1;
    }

    for(i = CACHE_START; i <= CACHE_END; i++) {
        obj = char_cache + (i - CACHE_START);
        type(obj) = CHARACTER;
        obj_cv(obj) = i;
    }
    return 0;
}

void charcache_dispose(void) {
    sc_free(char_cache);
    char_cache = NULL;
}

object* charcache_get(int c) {
    if (c >= CACHE_START && c <= CACHE_END) {
        return char_cache + (c - CACHE_START);
    }
    return NULL;
}

