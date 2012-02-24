#ifndef _SC_LOG_H_
#define _SC_LOG_H_

#include <stdio.h>
#include "config.h"

#ifdef ENABLE_LOG
#define sc_log(fmt, args...) fprintf(stderr, "[LOG:" __FILE__ "]" fmt "\n", args)
#else
#define sc_log(msg)
#endif

#endif
