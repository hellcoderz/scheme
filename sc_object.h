#ifndef _SC_OBJECT_H_
#define _SC_OBJECT_H_

#include <stdio.h>

#define NUL '\0'
#define TAB '\t'
#define LINEFEED '\n'
#define NEWLINE '\n'
#define RETURN '\r'
#define SPACE ' '

typedef enum {
	FIXNUM,
    BOOLEAN,
    CHARACTER,
    STRING,
    THE_EMPTY_LIST,
    PAIR,
    SYMBOL,
    PRIMITIVE_PROC,
    COMPOUND_PROC,
    INPUT_PORT,
    OUTPUT_PORT,
    EOF_OBJECT,
} object_type;

struct object;
typedef int (*prim_proc)(struct object *params, 
                         struct object **result);

typedef struct object {
    object_type type;
    union {
        struct {
            char value;
        } character;
        struct {
            char value;
        } boolean;
        struct {
            long value;
        } fixnum;
        struct {
            char *buf;
        } string;
        struct {
            struct object *car;
            struct object *cdr;
        } pair;
        struct {
            char *value;
        } symbol;
        struct {
            prim_proc fn;
        } primitive_proc;
        struct {
            struct object *parameters;
            struct object *body;
            struct object *env;
        } compound_proc;
        struct {
            FILE *stream;
        } input_port;
        struct {
            FILE *stream;
        } output_port;
    } data;
} object;

#define type(p) (p->type)
#define obj_cv(p) (p->data.character.value)
#define obj_bv(p) (p->data.boolean.value)
#define obj_nv(p) (p->data.fixnum.value)
#define obj_sv(p) (p->data.string.buf)
#define obj_pv(p) (p->data.pair)
#define obj_iv(p) (p->data.symbol.value)    /* identifier */
#define obj_fv(p) (p->data.primitive_proc.fn)
#define obj_lv(p) (p->data.compound_proc)  /* lambda */
#define obj_lvp(p) (obj_lv(p).parameters)
#define obj_lvb(p) (obj_lv(p).body)
#define obj_lve(p) (obj_lv(p).env)
#define obj_ipv(p) (p->data.input_port.stream)
#define obj_opv(p) (p->data.output_port.stream)


#define caar(obj)   car(car(obj))
#define cadr(obj)   car(cdr(obj))
#define cdar(obj)   cdr(car(obj))
#define cddr(obj)   cdr(cdr(obj))
#define caaar(obj)  car(car(car(obj)))
#define caadr(obj)  car(car(cdr(obj)))
#define cadar(obj)  car(cdr(car(obj)))
#define caddr(obj)  car(cdr(cdr(obj)))
#define cdaar(obj)  cdr(car(car(obj)))
#define cdadr(obj)  cdr(car(cdr(obj)))
#define cddar(obj)  cdr(cdr(car(obj)))
#define cdddr(obj)  cdr(cdr(cdr(obj)))
#define caaaar(obj) car(car(car(car(obj))))
#define caaadr(obj) car(car(car(cdr(obj))))
#define caadar(obj) car(car(cdr(car(obj))))
#define caaddr(obj) car(car(cdr(cdr(obj))))
#define cadaar(obj) car(cdr(car(car(obj))))
#define cadadr(obj) car(cdr(car(cdr(obj))))
#define caddar(obj) car(cdr(cdr(car(obj))))
#define cadddr(obj) car(cdr(cdr(cdr(obj))))
#define cdaaar(obj) cdr(car(car(car(obj))))
#define cdaadr(obj) cdr(car(car(cdr(obj))))
#define cdadar(obj) cdr(car(cdr(car(obj))))
#define cdaddr(obj) cdr(car(cdr(cdr(obj))))
#define cddaar(obj) cdr(cdr(car(car(obj))))
#define cddadr(obj) cdr(cdr(car(cdr(obj))))
#define cdddar(obj) cdr(cdr(cdr(car(obj))))
#define cddddr(obj) cdr(cdr(cdr(cdr(obj))))

int init_obj(void);
void dispose_obj(void);

object* make_fixnum(long value);
int is_fixnum(object *obj);

object* make_boolean(int value);
object* get_false_obj(void);
object* get_true_obj(void);
int boolean_init(void);
int is_boolean(object *obj);
int is_true(object *obj);
int is_false(object *obj);

object* make_character(int val);
int is_character(object *obj);

object* make_string(char *str);
int is_string(object *obj);
int string_init(void);
void string_dispose(void);

object* get_empty_list();
int is_empty_list(object *obj);
int empty_list_init();

int is_pair(object *obj);
object* cons(object *car, object *cdr);
object* car(object *pair);
object* cdr(object *pair);
int set_car(object *pair, object *car);
int set_cdr(object *pair, object *cdr);

object* make_symbol(char *sym);
int is_symbol(object *obj);
int symbol_init();
void symbol_dispose(void);

object* make_primitive_proc(prim_proc fn);
int is_primitive_proc(object *obj);

object* make_compound_proc(object *params, object *body, object *env);
int is_compound_proc(object *obj);

object* get_eof_object(void);
int is_eof_object(object *obj);
int eof_init(void);

object* make_input_port(FILE *stream);
int is_input_port(object *obj);

object* make_output_port(FILE *stream);
int is_output_port(object *obj);

#endif

