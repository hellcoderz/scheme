#ifndef _CHARCACHE_H_
#define _CHARCACHE_H_

#include "object.h"

int charcache_init(void);
void charcache_dispose(void);
object* charcache_get(int c);

#endif

