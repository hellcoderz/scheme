#ifndef _SC_LOG_H_
#define _SC_LOG_H_

// Undefine this line to turn off logging
#define DEBUG_LOG

void _sc_log(char *msg);

#ifdef DEBUG_LOG
#define sc_log(msg) \
    _sc_log(msg)
#elif
#define sc_log(msg)
#endif

#endif
