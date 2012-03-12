#include <string.h>
#include "object.h"
#include "objstream.h"
#include "gc.h"
#include "mem.h"

#define DEFAULT_CAPACITY 128
/**
 * PLEASE note:
 * no gc_protect/gc_abandon call when using objstream
 */

objstream* objstream_new(int cap) {
    int capacity = DEFAULT_CAPACITY;
    object **buf;
    objstream *stream;

    if (cap > 0) {
        capacity = cap;
    }

    buf = sc_malloc(sizeof(object*) * capacity);
    if (buf == NULL) {
        return NULL;
    }
    stream = sc_malloc(sizeof(objstream));
    if (stream == NULL) {
        sc_free(buf);
        return NULL;
    }

    stream->buf = buf;
    stream->capacity = capacity;
    stream->len = 0;
    return stream;
}

void abandon_objects(int len) {
    int i;

    for (i = 0; i < len; i++) {
        gc_abandon();
    }
}

void objstream_dispose(objstream *stream) {
    if (stream == NULL) {
        return;
    }
    if (stream->buf != NULL) {
        abandon_objects(stream->len);
        sc_free(stream->buf);
    }
    sc_free(stream);
    stream->buf = NULL;
}

int objstream_append(objstream *stream, object *obj) {
    int len, cap, newlen;

    if (stream == NULL) {
        return -1;
    }

    len = stream->len;
    cap = stream->capacity;
    newlen = len + 1;
    if (newlen > cap) {
        /* resize */
        int i;
        cap = cap * 2;
        object **buf = stream->buf;
        abandon_objects(len);
        buf = sc_realloc(buf, sizeof(object*) * cap);
        if (buf == NULL) {
            /* handle gc_abandon */
            sc_free(stream->buf);
            stream->buf = NULL;
            return -1;
        }
        /* push new address */
        for (i = 0; i < len; i++) {
            gc_protect(buf[i]);
        }
        stream->buf = buf;
        stream->capacity = cap;
    }

    stream->buf[len] = obj;
    gc_protect(stream->buf[len]);
    stream->len = newlen;
    return 0;
}

/* call sc_free() to free returned array */
object** objstream_trim(objstream *stream, int *plen) {
    object **buf;
    int len;

    if (stream == NULL) {
        return NULL;
    }

    len = stream->len;
    *plen = len;
    buf = sc_malloc(len * sizeof(object*));
    if (buf == NULL) {
        return NULL;
    }
    memcpy(buf, stream->buf, len * sizeof(object*));
    return buf;
}

