#ifndef _SC_ENV_H_
#define _SC_ENV_H_

#include "object.h"

int define_variable(object *var, object *val, object *env);
int set_variable_val(object *var, object *val, object *env);
object* lookup_variable_val(object *var, object *env);
object* extend_env(object *vars, object *vals, object *base_env);

object* make_null_env(void);
object* make_base_env(void);

int is_valid_env(object *env);

#endif

