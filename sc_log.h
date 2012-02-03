#ifndef _SC_LOG_H_
#define _SC_LOG_H_

#include "sc_config.h"

void _sc_log(char *msg);

#ifdef DEBUG_LOG
#define sc_log(msg) _sc_log(msg)
#else
#define sc_log(msg)
#endif

#endif
