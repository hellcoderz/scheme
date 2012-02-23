#include <string.h>
#include "mem.h"
#include "sstream.h"
#include "log.h"

sstream* sstream_new(int capacity) {
    sstream *stream;
    int cap = DEFAULT_CAPACITY;
    char *buf;

    stream = sc_malloc(sizeof(sstream));
    if (stream == NULL) {
        sc_log("out of memory[sstream]\n");
        return NULL;
    }

    if (capacity > 0) {
        cap = capacity;
    }
    buf = sc_malloc(cap);
    if (buf == NULL) {
        sc_log("out of memory[sstream]\n");
        sc_free(stream);
        return NULL;
    }

    stream->buf = buf;
    stream->capacity = cap;
    stream->len = 0;
    return stream;
}

void sstream_dispose(sstream *stream) {
    if (stream == NULL) {
        return;
    }

    sc_free(stream->buf);
    stream->buf = NULL;
    sc_free(stream);
}

int sstream_append(sstream *stream, char c) {
    int newlen = stream->len + 1;
    int cap = stream->capacity;
    char *buf = stream->buf;

    if (newlen > cap) {
        cap *= 2;
        buf = sc_realloc(buf, cap);
        if (buf == NULL) {
            sc_log("can not resize stream, no memory\n");
            return -1;
        }
        stream->capacity = cap;
        stream->buf = buf;
    }

    buf[newlen - 1] = c;
    stream->len = newlen;
    return 0;
}

char* sstream_cstr(sstream *stream) {
    char *str;
    int len = stream->len;

    str = sc_malloc(len + 1);
    if (str == NULL) {
        sc_log("no memory for cstr\n");
        return NULL;
    }

    strncpy(str, stream->buf, len);
    str[len] = '\0';
    return str;
}

