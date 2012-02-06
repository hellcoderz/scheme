#ifndef _SC_SSTREAM_H_
#define _SC_SSTREAM_H_

#define DEFAULT_CAPACITY 256;

typedef struct {
    char *buf;
    int capacity;
    int len;
} sstream;

sstream* sstream_new(int capacity);
void sstream_dispose(sstream *stream);
int sstream_append(sstream *stream, char c);
char* sstream_cstr(sstream *stream);

#endif

