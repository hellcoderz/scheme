#ifndef _INTCACHE_H_
#define _INTCACHE_H_

#include "object.h"

int intcache_init(void);
void intcache_dispose(void);
object* intcache_get(long val);

#endif

