#ifndef _SC_ENV_H_
#define _SC_ENV_H_

#include "sc_object.h"

object* get_global_env(void);
int setup_env(void);
int define_variable(object *var, object *val, object *env);
int set_variable_val(object *var, object *val, object *env);
object* lookup_variable_val(object *var, object *env);
object* extend_env(object *vars, object *vals, object *base_env);

#endif

