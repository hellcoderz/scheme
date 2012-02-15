#include <stdio.h>
#include "sc_eval.h"
#include "sc_object.h"
#include "sc_log.h"
#include "sc_sform.h"
#include "sc_env.h"
#include "sc_procdef.h"

static int is_self_evaluate(object *exp) {
    return is_fixnum(exp) ||
           is_boolean(exp) ||
           is_character(exp) ||
           is_string(exp);
}

static int is_variable(object *exp) {
    return is_symbol(exp);
}

static int is_tagged_list(object *exp, object *tag) {
    object *car_obj;

    if (is_pair(exp)) {
        car_obj = car(exp);
        return is_symbol(car_obj) && car_obj == tag;
    }
    return 0;
}

static int is_quoted(object *exp) {
    return is_tagged_list(exp, get_quote_symbol());
}

static object* eval_quote(object *exp) {
    return cadr(exp);
}

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
        fprintf(stderr, "%s\n", "variable must be symbol");
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

static object* make_lambda(object *parameters, object *body) {
    return cons(get_lambda_symbol(),
                cons(parameters, body));
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

static object* make_begin(object *seq) {
    return cons(get_begin_symbol(), seq);
}

static int is_begin(object *exp) {
    return is_tagged_list(exp, get_begin_symbol());
}

static object* begin_actions(object *exp) {
    return cdr(exp);
}

static int is_last_exp(object *exp) {
    return is_empty_list(cdr(exp));
}

static object* first_exp(object *exp) {
    return car(exp);
}

static object* rest_exps(object *exp) {
    return cdr(exp);
}

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

    val = sc_eval(val, env);
    if (val == NULL) {
        return NULL;
    }
    ret = define_variable(var, val, env);
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
        fprintf(stderr, "%s\n",
                "unbound variable");
    }
    return obj;
}

static object* make_if(object *predicate,
                       object *consequence,
                       object *alternative) {
    return cons(get_if_symbol(),
                cons(predicate,
                    cons(consequence,
                        cons(alternative, get_empty_list()))));
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
    if (is_no_operands(ops)) {
        return get_empty_list();
    } else {
        return cons(sc_eval(first_operand(ops), env),
                    list_of_values(rest_operands(ops), env));
    }
}

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
                return make_if(cond_predicate(first),
                               sequence_to_exp(cond_actions(first)),
                               alter);
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
        return cons(var, rest);
    }
}

static object* bindings_values(object *bindings) {
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
        return cons(val, rest);
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

    vars = let_variables(exp);
    if (vars == NULL) {
        return NULL;
    }
    vals = let_values(exp);
    if (vals == NULL) {
        return NULL;
    }
    body = let_body(exp);
    if (body == NULL || is_empty_list(body)) {
        return NULL;
    }
    return make_application(
                make_lambda(vars, body),
                vals);
}

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
        return cons(car(args),
                    normalize_apply_operands(cdr(args)));
    }

    /* never here */
    return NULL;
}

static object* apply_operands(object *args) {
    return normalize_apply_operands(cdr(args));
}


object* sc_eval(object *exp, object *env) {
    object *val;

tailcall:
    if (exp == NULL) {
        sc_log("cannot eval NULL exp\n");
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
    } else if (is_if(exp)) {
        object *pred;
        if (!check_if_arity(exp)) {
            fprintf(stderr, "%s\n",
                    "wrong arity in `if form");
            return NULL;
        }
        pred = sc_eval(if_predicate(exp), env);
        exp = is_true(pred) ? if_consequence(exp) : if_alternative(exp);
        goto tailcall;
    } else if (is_lambda(exp)) {
        return make_compound_proc(lambda_parameters(exp),
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
            return NULL;
        }
        goto tailcall;
    } else if (is_let(exp)) {
        exp = let_to_application(exp);
        if (exp == NULL) {
            fprintf(stderr, "malformed let form\n");
            return NULL;
        }
        goto tailcall;
    } else if (is_and(exp)) {
        object *result;
        exp = and_tests(exp);
        if (is_empty_list(exp)) {
            return get_true_obj();
        }
        while (!is_last_exp(exp)) {
            result = sc_eval(first_exp(exp), env);
            if (is_false(result)) {
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
            return get_false_obj();
        }
        while (!is_last_exp(exp)) {
            result = sc_eval(first_exp(exp), env);
            if (is_true(result)) {
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
        args = list_of_values(operands(exp), env);

        /* handle apply specially for tailcall requirement */
        if (is_apply(op)) {
            char msg[] = "wrong arity in apply";
            op = apply_operator(args);
            if (op == NULL) {
                fprintf(stderr, "%s\n", msg);
                return NULL;
            }
            args = apply_operands(args);
            if (args == NULL) {
                fprintf(stderr, "%s\n", msg);
                return NULL;
            }
        }

        if (is_primitive_proc(op)) {
            fn = obj_fv(op);
            if (fn == NULL) {
                sc_log("invalid primitive procedure\n");
                return NULL;
            }
            err = fn(args, &ret);
            if (err != 0) {
                fprintf(stderr, "%s\n", error_str(err));
                return NULL;
            }
            return ret;
        } else if (is_compound_proc(op)) {
            env = extend_env(obj_lvp(op),
                             args,
                             obj_lve(op));
            exp = make_begin(obj_lvb(op));
            goto tailcall;
        } else {
            fprintf(stderr, "%s\n", "not applicable");
            return NULL;
        }
    } else {
        val = NULL;
        fprintf(stderr,
                "cannot evaluate expression\n");
    }

    return val;
}

