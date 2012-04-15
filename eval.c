#include <stdio.h>
#include "eval.h"
#include "object.h"
#include "log.h"
#include "sform.h"
#include "env.h"
#include "procdef.h"
#include "gc.h"
#include "write.h"

static int is_self_evaluate(object *exp) {
    return is_fixnum(exp) ||
           is_flonum(exp) ||
           is_boolean(exp) ||
           is_character(exp) ||
           is_string(exp) ||
           is_env_frame(exp) ||
           is_cont(exp) ||
           is_primitive_proc(exp) ||
           is_compound_proc(exp) ||
           is_eof_object(exp);
}

static int is_variable(object *exp) {
    return is_symbol(exp);
}

static int is_tagged_list(object *exp, object *tag) {
    object *car_obj;

    if (is_pair(exp)) {
        car_obj = car(exp);
        return car_obj == tag;
    }
    return 0;
}

/* eval quote */
static int is_quoted(object *exp) {
    return is_tagged_list(exp, get_quote_symbol());
}

static object* eval_quote(object *exp) {
    return cadr(exp);
}

/* assignment functions */
static int is_assignment(object *exp) {
    return is_tagged_list(exp, get_set_symbol());
}

static object* assignment_variable(object *exp) {
    return cadr(exp);
}

static object* assignment_value(object *exp) {
    return caddr(exp);
}

static object* eval_assignment(object *exp, object *env) {
    object *var, *val;
    char msg[] = "wrong arity in `set! form\n";
    int ret;

    var = assignment_variable(exp);
    val = assignment_value(exp);
    if (val == NULL || var == NULL || 
        !is_empty_list(cdddr(exp))) {
        fprintf(stderr, "%s", msg);
        return NULL;
    }
    if (!is_variable(var)) {
        fprintf(stderr, "%s\n", "variable name must be symbol");
        return NULL;
    }

    val = sc_eval(val, env);
    if (val == NULL) {
        return NULL;
    }
    ret = set_variable_val(var, val, env);
    if (ret != 0) {
        fprintf(stderr, "%s\n", "unbound variable");
        return NULL;
    }
    return get_nrv_symbol();
}

/* lambda functions */
static object* make_lambda(object *parameters, object *body) {
    object *obj;

    obj = cons(parameters, body);
    gc_protect(obj);
    obj = cons(get_lambda_symbol(), obj);
    gc_abandon();
    return obj;
}

static int is_lambda(object *exp) {
    return is_tagged_list(exp, get_lambda_symbol());
}

static object* lambda_parameters(object *exp) {
    return cadr(exp);
}

static object* lambda_body(object *exp) {
    return cddr(exp);
}

static int check_arity(object *args, int arity, int has_vararg) {
    int i;
    object *rest = args;

    for (i = 0; i < arity; i++) {
        rest = cdr(rest);
    }

    if (!has_vararg && !is_empty_list(rest)) {
        return 0;
    }
    if (has_vararg && rest == NULL) {
        return 0;
    }
    return 1;
}

static object* normalize_lambda_args(int argc, int has_vararg, object *raw_arg) {
    object *norm_args = raw_arg;

    if (!check_arity(raw_arg, argc, has_vararg)) {
        return NULL;
    }

    if (has_vararg) {
        if (argc == 0) {
            norm_args = cons(raw_arg, get_empty_list());
        } else {
            object *cdr_obj = get_empty_list();
            while (argc != 1) {
                raw_arg = cdr(raw_arg);
                argc--;
            }
            cdr_obj = cons(cdr(raw_arg), cdr_obj);
            set_cdr(raw_arg, cdr_obj);
        }
    }
    return norm_args;
}

/* begin form functions */
static object* make_begin(object *seq) {
    return cons(get_begin_symbol(), seq);
}

static int is_begin(object *exp) {
    return is_tagged_list(exp, get_begin_symbol());
}

static object* begin_actions(object *exp) {
    return cdr(exp);
}

/* expression related common functions */
static int is_last_exp(object *exp) {
    return is_empty_list(cdr(exp));
}

static object* first_exp(object *exp) {
    return car(exp);
}

static object* rest_exps(object *exp) {
    return cdr(exp);
}

/* define form funtions */
static int is_definition(object *exp) {
    return is_tagged_list(exp, get_define_symbol());
}

static object* definition_variable(object *exp) {
    if (is_symbol(cadr(exp))) {
        return cadr(exp);
    } else {
        /* lambda syntax sugar */
        return caadr(exp);
    }
}

static object* definition_value(object *exp) {
    if (is_symbol(cadr(exp))) {
        return caddr(exp);
    } else {
        /* lambda syntax sugar */
        return make_lambda(cdadr(exp), cddr(exp));
    }
}

static object* eval_definition(object *exp, object *env) {
    object *var, *val;
    char msg[] = "wrong arity in `define form\n";
    int ret;

    var = definition_variable(exp);
    val = definition_value(exp);
    if (val == NULL || var == NULL || 
        (is_symbol(cadr(exp)) && !is_empty_list(cdddr(exp)))) {
        fprintf(stderr, "%s", msg);
        return NULL;
    }
    if (!is_variable(var)) {
        fprintf(stderr, "%s\n", "variable must be symbol");
        return NULL;
    }

    gc_protect(val); /* protect lambda form */
    val = sc_eval(val, env);
    if (val == NULL) {
        return NULL;
    }
    ret = define_variable(var, val, env);
    gc_abandon();
    if (ret != 0) {
        fprintf(stderr, "%s\n",
                "unexpected error, cannot define variable");
        return NULL;
    }
    return get_nrv_symbol();
}

static object* eval_variable(object *exp, object *env) {
    object *obj;

    obj = lookup_variable_val(exp, env);
    if (obj == NULL) {
        fprintf(stderr, "%s `%s\n",
                "unbound variable", obj_iv(exp));
    }
    return obj;
}

/* if form functions */
static object* make_if(object *predicate,
                       object *consequence,
                       object *alternative) {
    object *obj;

    obj = cons(alternative, get_empty_list());
    gc_protect(obj);
    obj = cons(consequence, obj);
    obj = cons(predicate, obj);
    obj = cons(get_if_symbol(), obj);
    gc_abandon();

    return obj;
}

static int is_if(object *exp) {
    return is_tagged_list(exp, get_if_symbol());
}

static object* if_predicate(object *exp) {
    return cadr(exp);
}

static object* if_consequence(object *exp) {
    return caddr(exp);
}

static object* if_alternative(object *exp) {
    object *alter;
    
    alter = cdddr(exp);
    if (is_empty_list(alter)) {
        return get_false_obj(); /* undefined */
    } else {
        return car(alter);
    }

    return NULL;
}

static int check_if_arity(object *exp) {
    return is_empty_list(cdddr(exp)) ||
           is_empty_list(cddddr(exp));
}

/* function application */
static object* make_application(object *operator, object *operands) {
    return cons(operator, operands);
}

static int is_application(object *exp) {
    return is_pair(exp);
}

static object* operator(object *exp) {
    return car(exp);
}

static object* operands(object *exp) {
    return cdr(exp);
}

static int is_no_operands(object *ops) {
    return is_empty_list(ops);
}

static object* first_operand(object *ops) {
    return car(ops);
}

static object* rest_operands(object *ops) {
    return cdr(ops);
}

static object* list_of_values(object *ops, object *env) {
    object *car_obj, *cdr_obj, *obj;

    if (is_no_operands(ops)) {
        return get_empty_list();
    } else {
        car_obj = sc_eval(first_operand(ops), env);
        if (car_obj == NULL) {
            return NULL;
        }
        gc_protect(car_obj);
        cdr_obj = list_of_values(rest_operands(ops), env);
        if (cdr_obj == NULL) {
            gc_abandon();
            return NULL;
        }
        gc_protect(cdr_obj);
        obj = cons(car_obj, cdr_obj);
        gc_abandon();
        gc_abandon();
        
        return obj;
    }
}

/* cond form functions */
static object* cond_clauses(object *exp) {
    return cdr(exp);
}

static int is_cond(object *exp) {
    return is_tagged_list(exp, get_cond_symbol());
}

static object* cond_predicate(object *clause) {
    return car(clause);
}

static object* cond_actions(object *clause) {
    return cdr(clause);
}

static int is_cond_else_clause(object *clause) {
    return cond_predicate(clause) == get_else_symbol();
}

static object* sequence_to_exp(object *seq) {
    if (is_empty_list(seq)) {
        return seq;
    } else if (is_last_exp(seq)) {
        return car(seq);
    } else {
        return make_begin(seq);
    }
}

static object* expand_clauses(object *clauses) {
    object *first, *rest;

    if (is_empty_list(clauses)) {
        return get_false_obj();
    } else {
        first = car(clauses);
        rest = cdr(clauses);
        if (is_cond_else_clause(first)) {
            if (is_empty_list(rest)) {
                return sequence_to_exp(cond_actions(first));
            } else {
                fprintf(stderr,
                        "else clause isn't last cond->if\n");
                return NULL;
            }
        } else {
            object *alter;
            alter = expand_clauses(rest);
            if (alter == NULL) {
                return NULL;
            } else {
                object *exps, *if_obj;
                gc_protect(alter);
                exps = sequence_to_exp(cond_actions(first));
                gc_protect(exps);
                if_obj = make_if(cond_predicate(first),
                                 exps,
                                 alter);
                gc_abandon();
                gc_abandon();
                return if_obj;
            }
        }
    }
}

static object* cond_to_if(object *exp) {
    object *clauses;

    clauses = cond_clauses(exp);
    if (is_empty_list(clauses)) {
        fprintf(stderr, "no clauses in cond form\n");
        return NULL;
    }
    return expand_clauses(clauses);
}

/* let form functions */
static int is_let(object *exp) {
    return is_tagged_list(exp, get_let_symbol());
}

static object* let_bindings(object *exp) {
    return cadr(exp);
}

static object* let_body(object *exp) {
    return cddr(exp);
}

static object* binding_variable(object *binding) {
    return car(binding);
}

static object* binding_value(object *binding) {
    return cadr(binding);
}

static int is_valid_binding(object *binding) {
    return is_empty_list(cddr(binding));
}

static object* bindings_variables(object *bindings) {
    object *list;

    if (is_empty_list(bindings)) {
        return get_empty_list();
    } else {
        object *binding;
        object *var, *rest;

        binding = car(bindings);
        if (!is_valid_binding(binding)) {
            return NULL;
        }
        var = binding_variable(binding);
        rest = bindings_variables(cdr(bindings));
        if (rest == NULL) {
            return NULL;
        }

        gc_protect(rest);
        list = cons(var, rest);
        gc_abandon();
        return list;
    }
}

static object* bindings_values(object *bindings) {
    object *list;

    if (is_empty_list(bindings)) {
        return get_empty_list();
    } else {
        object *binding;
        object *val, *rest;

        binding = car(bindings);
        if (!is_valid_binding(binding)) {
            return NULL;
        }
        val = binding_value(binding);
        rest = bindings_values(cdr(bindings));
        if (rest == NULL) {
            return NULL;
        }

        gc_protect(rest);
        list = cons(val, rest);
        gc_abandon();
        return list;
    }
}

static int is_valid_let_bindings(object *bindings) {
    return is_pair(bindings);
}

static object* let_variables(object *exp) {
    object *bindings;

    bindings = let_bindings(exp);
    if (!is_valid_let_bindings(bindings)) {
        return NULL;
    }
    return bindings_variables(bindings);
}

static object* let_values(object *exp) {
    object *bindings;

    bindings = let_bindings(exp);
    if (!is_valid_let_bindings(bindings)) {
        return NULL;
    }
    return bindings_values(bindings);
}

static object* let_to_application(object *exp) {
    object *vars, *vals;
    object *body;
    object *lambda_obj, *result;

    vars = let_variables(exp);
    if (vars == NULL) {
        return NULL;
    }
    gc_protect(vars);
    vals = let_values(exp);
    if (vals == NULL) {
        return NULL;
    }
    body = let_body(exp);
    if (body == NULL || is_empty_list(body)) {
        return NULL;
    }
    gc_protect(vals);
    lambda_obj = make_lambda(vars, body);
    gc_protect(lambda_obj);
    result = make_application(lambda_obj, vals);
    gc_abandon();
    gc_abandon();
    gc_abandon();
    return result;
}

/* and, or form functions */
static int is_and(object *exp) {
    return is_tagged_list(exp, get_and_symbol());
}

static int is_or(object *exp) {
    return is_tagged_list(exp, get_or_symbol());
}

static object* and_tests(object *exp) {
    return cdr(exp);
}

static object* or_tests(object *exp) {
    return cdr(exp);
}

static object* apply_operator(object *args) {
    return car(args);
}

/* apply form functions */
static object* normalize_apply_operands(object *args) {
    if (is_empty_list(args)) {
        return NULL;
    }
    if (is_empty_list(cdr(args))) {
        object *obj;
        obj = car(args);
        if (is_empty_list(obj) || is_pair(obj)) {
            return obj;
        } else {
            return cons(obj, get_empty_list());
        }
    } else {
        object *obj, *result;
        obj = normalize_apply_operands(cdr(args));
        gc_protect(obj);
        result = cons(car(args), obj);
        gc_abandon();
        return result;
    }

    /* never here */
    return NULL;
}

static object* apply_operands(object *args) {
    return normalize_apply_operands(cdr(args));
}

/* eval form functions */
static object* eval_exps(object *args) {
    return car(args);
}

/* macro functions */
static int is_macro_definition(object *exp) {
    return is_tagged_list(exp, get_definemacro_symbol());
}

static object *eval_macro_definition(object *exp, object *env) {
    object *var, *val;
    char msg[] = "wrong arity in `define-macro form\n";
    int ret;

    var = definition_variable(exp);
    val = definition_value(exp);
    if (val == NULL || var == NULL || 
        (is_symbol(cadr(exp)) && !is_empty_list(cdddr(exp)))) {
        fprintf(stderr, "%s", msg);
        return NULL;
    }
    if (!is_variable(var)) {
        fprintf(stderr, "%s\n", "variable must be symbol");
        return NULL;
    }

    gc_protect(val); /* protect lambda form */
    val = sc_eval(val, env);
    if (val == NULL) {
        return NULL;
    }
    val = make_macro(val);
    ret = define_variable(var, val, env);
    gc_abandon();
    if (ret != 0) {
        fprintf(stderr, "%s\n",
                "unexpected error in define-macro");
        return NULL;
    }
    return get_nrv_symbol();
}

static object* eval_env(object *args) {
    object *env;

    env = cadr(args);
    if (env == NULL) {
        fprintf(stderr, "%s\n",
                "wrong arity in eval");
        return NULL;
    }
    if (!is_valid_env(env)) {
        fprintf(stderr, "%s\n",
                "invalid environment");
        return NULL;
    }
    return env;
}


object* sc_eval(object *exp, object *env) {
    object *val = NULL;

    gc_protect(exp);
    gc_protect(env);
tailcall:
    if (exp == NULL) {
        sc_log("%s", "cannot eval NULL exp");
        return NULL;
    }

    if (is_self_evaluate(exp)) {
        val = exp;
    } else if (is_variable(exp)) {
        val = eval_variable(exp, env);
    } else if (is_quoted(exp)) {
        val = eval_quote(exp);
    } else if (is_assignment(exp)) {
        val = eval_assignment(exp, env);
    } else if (is_definition(exp)) {
        val = eval_definition(exp, env);
    } else if (is_macro_definition(exp)) {
        val = eval_macro_definition(exp, env);
    } else if (is_if(exp)) {
        object *pred;
        if (!check_if_arity(exp)) {
            fprintf(stderr, "%s\n",
                    "wrong arity in `if form");
            gc_abandon();
            gc_abandon();
            return NULL;
        }
        pred = sc_eval(if_predicate(exp), env);
        exp = is_true(pred) ? if_consequence(exp) : if_alternative(exp);
        goto tailcall;
    } else if (is_lambda(exp)) {
        val = make_compound_proc(lambda_parameters(exp),
                                  lambda_body(exp),
                                  env);
    } else if (is_begin(exp)) {
        exp = begin_actions(exp);
        while (!is_last_exp(exp)) {
            sc_eval(first_exp(exp), env);
            exp = rest_exps(exp);
        }
        exp = first_exp(exp);
        goto tailcall;
    } else if (is_cond(exp)) {
        exp = cond_to_if(exp);
        if (exp == NULL) {
            gc_abandon();
            gc_abandon();
            return NULL;
        }
        goto tailcall;
    } else if (is_let(exp)) {
        exp = let_to_application(exp);
        if (exp == NULL) {
            fprintf(stderr, "malformed let form\n");
            gc_abandon();
            gc_abandon();
            return NULL;
        }
        goto tailcall;
    } else if (is_and(exp)) {
        object *result;
        exp = and_tests(exp);
        if (is_empty_list(exp)) {
            gc_abandon();
            gc_abandon();
            return get_true_obj();
        }
        while (!is_last_exp(exp)) {
            result = sc_eval(first_exp(exp), env);
            if (is_false(result)) {
                gc_abandon();
                gc_abandon();
                return result;
            }
            exp = rest_exps(exp);
        }
        exp = first_exp(exp);
        goto tailcall;
    } else if (is_or(exp)) {
        object *result;
        exp = or_tests(exp);
        if (is_empty_list(exp)) {
            gc_abandon();
            gc_abandon();
            return get_false_obj();
        }
        while (!is_last_exp(exp)) {
            result = sc_eval(first_exp(exp), env);
            if (is_true(result)) {
                gc_abandon();
                gc_abandon();
                return result;
            }
            exp = rest_exps(exp);
        }
        exp = first_exp(exp);
        goto tailcall;
    } else if (is_application(exp)) {
        object *op, *args;
        prim_proc fn;
        object *ret;
        int err;

        op = sc_eval(operator(exp), env);
        if (is_macro(op)) {
            object *texp, *tenv;
            /* handle var-arg */
            op = obj_mv(op);
            args = cdr(exp);
            gc_protect(op);
            gc_protect(args);
            args = normalize_lambda_args(obj_lvargc(op), obj_lvvar(op), args);
            if (args == NULL) {
                fprintf(stderr, "wrong arity `");
                sc_write(stderr, exp);
                fprintf(stderr, "\n");
                gc_abandon();
                gc_abandon();
                gc_abandon();
                gc_abandon();
                return NULL;
            }

            /* do macro transform */
            tenv = extend_env(obj_lvp(op),
                             args,
                             obj_lve(op));
            texp = make_begin(obj_lvb(op));
            gc_protect(tenv);
            gc_protect(texp);
            exp = sc_eval(texp, tenv); 
            gc_abandon();
            gc_abandon();

            gc_abandon(); /* args */
            gc_abandon(); /* op */
            goto tailcall;
        }

        /* normal application */
        gc_protect(op);
        args = list_of_values(operands(exp), env);
        if (args == NULL) {
            gc_abandon();
            gc_abandon();
            gc_abandon();
            return NULL;
        }
        gc_protect(args);

        /* handle apply specially for tailcall requirement */
        if (is_apply(op)) {
            char msg[] = "wrong arity in apply";
            op = apply_operator(args);
            if (op == NULL) {
                fprintf(stderr, "%s\n", msg);
                gc_abandon();
                gc_abandon();
                gc_abandon();
                gc_abandon();
                return NULL;
            }
            args = apply_operands(args);
            if (args == NULL) {
                fprintf(stderr, "%s\n", msg);
                gc_abandon();
                gc_abandon();
                gc_abandon();
                gc_abandon();
                return NULL;
            }
        }

        /* handle eval specially for tailcall requirement */
        if (is_eval(op)) {
            char msg[] = "wrong arity in eval";
            exp = eval_exps(args);
            if (exp == NULL) {
                fprintf(stderr, "%s\n", msg);
                gc_abandon();
                gc_abandon();
                gc_abandon();
                gc_abandon();
                return NULL;
            }
            env = eval_env(args);
            if (env == NULL) {
                gc_abandon();
                gc_abandon();
                gc_abandon();
                gc_abandon();
                return NULL;
            }
            gc_abandon();
            gc_abandon();
            goto tailcall;
        }

        if (is_callwcc(op)) {
            object *c;
            if (!is_empty_list(cdr(args))) {
                fprintf(stderr, "wrong arity `");
                sc_write(stderr, exp);
                fprintf(stderr, "\n");
                gc_abandon();
                gc_abandon();
                gc_abandon();
                gc_abandon();
                return NULL;
            }
            c = save_cont();
            if (c != NULL) {
                /* continuation saved */
                gc_protect(c);
                c = cons(c, get_empty_list());
                c = cons(car(args), c);
                exp = c;
                gc_abandon();
                gc_abandon();
                gc_abandon();
                goto tailcall;
            } else {
                /* continuation returns */
                val = get_escape_val();
                gc_abandon();
                gc_abandon();
                gc_abandon();
                gc_abandon();
                return val;
            }
        }

        if (is_primitive_proc(op)) {
            fn = obj_fv(op);
            if (fn == NULL) {
                sc_log("%s", "invalid primitive procedure");
                gc_abandon();
                gc_abandon();
                gc_abandon();
                gc_abandon();
                return NULL;
            }
            err = fn(args, &ret);
            if (err != 0) {
                fprintf(stderr, "%s `", error_str(err));
                sc_write(stderr, exp);
                fprintf(stderr, "\n");
                gc_abandon();
                gc_abandon();
                gc_abandon();
                gc_abandon();
                return NULL;
            }
            gc_abandon();
            gc_abandon();
            val = ret;
        } else if (is_compound_proc(op)) {
            /* handle var-arg */
            args = normalize_lambda_args(obj_lvargc(op), obj_lvvar(op), args);
            if (args == NULL) {
                fprintf(stderr, "wrong arity `");
                sc_write(stderr, exp);
                fprintf(stderr, "\n");
                gc_abandon();
                gc_abandon();
                gc_abandon();
                gc_abandon();
                return NULL;
            }
            env = extend_env(obj_lvp(op),
                             args,
                             obj_lve(op));
            exp = make_begin(obj_lvb(op));
            gc_abandon();
            gc_abandon();
            goto tailcall;
        } else if (is_cont(op)) {
            if (!is_empty_list(cdr(args))) {
                fprintf(stderr, "wrong arity `");
                sc_write(stderr, exp);
                fprintf(stderr, "\n");
                gc_abandon();
                gc_abandon();
                gc_abandon();
                gc_abandon();
                return NULL;
            }
            /* never return, stack root is automatically abandoned*/
            restore_cont(obj_cont(op), car(args));
        } else {
            fprintf(stderr, "%s `", "object not applicable");
            sc_write(stderr, exp);
            fprintf(stderr, "\n");
            gc_abandon();
            gc_abandon();
            gc_abandon();
            gc_abandon();
            return NULL;
        }
    } else {
        val = NULL;
        fprintf(stderr,
                "cannot evaluate expression `");
        sc_write(stderr, exp);
        fprintf(stderr, "\n");
    }

    gc_abandon();
    gc_abandon();
    return val;
}

