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
    FLONUM,
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
    VECTOR,
    ENV_FRAME,
    CONT,
} object_type;

struct object;
typedef int (*prim_proc)(struct object *params, 
                         struct object **result);

typedef struct gc_head {
    int mark;
    struct object *chain;
} gc_head;

struct rbnode;
typedef struct rbnode *position;
typedef struct rbnode *rbtree;

struct cont;

typedef struct object {
    gc_head gc;
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
            double value;
        } flonum;
        struct {
            char *buf;
            int len;
        } string;
        struct {
            struct object *car;
            struct object *cdr;
        } pair;
        struct {
            char *value;
        } symbol;
        struct {
            struct object **array;
            int size;
        } vector;
        struct {
            prim_proc fn;
        } primitive_proc;
        struct {
            struct object *parameters;
            struct object *body;
            struct object *env;
            char has_vararg;
            unsigned char argc; /* (x y . z) is 2 */
        } compound_proc;
        struct {
            FILE *stream;
        } input_port;
        struct {
            FILE *stream;
        } output_port;
        struct {
           rbtree tree;
        } env_frame;
        struct {
            struct cont *c;
        } continuation;
    } data;
} object;

#define type(p) (p->type)
#define obj_cv(p) (p->data.character.value)
#define obj_bv(p) (p->data.boolean.value)
#define obj_nv(p) (p->data.fixnum.value)
#define obj_rv(p) (p->data.flonum.value) /* rational number */
#define obj_sv(p) (p->data.string.buf)
#define obj_slenv(p) (p->data.string.len)
#define obj_vav(p) (p->data.vector.array)
#define obj_vsv(p) (p->data.vector.size)
#define obj_pv(p) (p->data.pair)
#define obj_iv(p) (p->data.symbol.value)    /* identifier */
#define obj_fv(p) (p->data.primitive_proc.fn)
#define obj_lv(p) (p->data.compound_proc)  /* lambda */
#define obj_lvp(p) (obj_lv(p).parameters)
#define obj_lvb(p) (obj_lv(p).body)
#define obj_lve(p) (obj_lv(p).env)
#define obj_lvargc(p) (obj_lv(p).argc)
#define obj_lvvar(p) (obj_lv(p).has_vararg)
#define obj_ipv(p) (p->data.input_port.stream)
#define obj_opv(p) (p->data.output_port.stream)
#define gc_mark(p) ((p)->gc.mark)
#define gc_chain(p) ((p)->gc.chain)
#define obj_rbtv(p) (p->data.env_frame.tree)
#define obj_cont(p) (p->data.continuation.c)

#define SIZEOF_OBJECT   sizeof(object)

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

object* make_flonum(double val);
int is_flonum(object *obj);

object* make_boolean(int value);
object* get_false_obj(void);
object* get_true_obj(void);
int boolean_init(void);
int is_boolean(object *obj);
int is_true(object *obj);
int is_false(object *obj);
void boolean_dispose(void);

object* make_character(int val);
int is_character(object *obj);

object* make_string(char *str);
int is_string(object *obj);
void string_free(object *obj);

object* get_empty_list(void);
int is_empty_list(object *obj);
int empty_list_init(void);
void empty_list_dispose(void);

int is_pair(object *obj);
object* cons(object *car, object *cdr);
object* car(object *pair);
object* cdr(object *pair);
int set_car(object *pair, object *car);
int set_cdr(object *pair, object *cdr);

int is_vector(object *obj);
object* make_vector(object **arr, int size);
void vector_free(object *obj);

object* make_symbol(char *sym);
int is_symbol(object *obj);
int symbol_init();
void symbol_dispose(void);
void symbol_free(object *obj);

object* make_primitive_proc(prim_proc fn);
int is_primitive_proc(object *obj);

object* make_compound_proc(object *params, object *body, object *env);
int is_compound_proc(object *obj);

object* get_eof_object(void);
int is_eof_object(object *obj);
int eof_init(void);
void eof_dispose(void);

object* make_input_port(FILE *stream);
int is_input_port(object *obj);
object* make_output_port(FILE *stream);
int is_output_port(object *obj);
void port_free(object *obj);


typedef void (*env_frame_walk_fn)(object *var, object *val);
object* make_env_frame(void);
int is_env_frame(object *obj);
int env_frame_init(void);
void env_frame_dispose(void);
int env_frame_insert(object *frame, object *var, object *val);
int env_frame_change(object *frame, object *var, object *val);
object* env_frame_find(object *frame, object *var);
void env_frame_walk(object *frame, env_frame_walk_fn walker);
void env_frame_free(object *obj);

int is_cont(object *obj);
object *save_cont(void);
void internal_restore_cont(struct cont *c, object *val, int once_more);
void cont_free(object *obj);
object* get_escape_val(void);
#define restore_cont(c, v) internal_restore_cont(c, v, 1);

#endif

