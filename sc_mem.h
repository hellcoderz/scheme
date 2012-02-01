#ifndef _SC_MEM_H_
#define _SC_MEM_H_

#include "sc_object.h"

// General purpose memory alloction functions
void* sc_malloc(size_t size);
void sc_free(void *ptr);
void* sc_realloc(void *ptr, size_t size);

// Object related functions
object* alloc_object(void);
object* make_fixnum(long value);
bool is_fixnum(object *obj);

#endif

