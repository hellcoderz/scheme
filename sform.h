#ifndef _SC_SFORM_H_
#define _SC_SFORM_H_

#include "object.h"

int sform_init(void);

object* get_quote_symbol(void);
object* get_set_symbol(void);
object* get_define_symbol(void);
object* get_if_symbol(void);
object* get_nrv_symbol(void);
object* get_lambda_symbol(void);
object* get_begin_symbol(void);
object* get_cond_symbol(void);
object* get_else_symbol(void);
object* get_let_symbol(void);
object* get_and_symbol(void);
object* get_or_symbol(void);
object* get_callwcc_symbol(void);

#endif

