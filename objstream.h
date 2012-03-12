#ifndef _OBJSTREAM_H_
#define _OBJSTREAM_H_

typedef struct {
    object **buf;
    int capacity;
    int len;
} objstream;

objstream* objstream_new(int cap);
int objstream_append(objstream *stream, object *obj);
void objstream_dispose(objstream *stream);
object** objstream_trim(objstream *stream, int *len);

#endif

