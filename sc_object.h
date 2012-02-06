#ifndef _SC_OBJECT_H_
#define _SC_OBJECT_H_

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
} object_type;

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
    } data;
} object;

#define type(p) (p->type)
#define obj_cv(p) (p->data.character.value)
#define obj_bv(p) (p->data.boolean.value)
#define obj_nv(p) (p->data.fixnum.value)
#define obj_sv(p) (p->data.string.buf)
#define obj_pv(p) (p->data.pair)

extern object *g_true_val;
extern object *g_false_val;
extern object *g_the_empty_list;

int init_obj(void);

object* make_fixnum(long value);
int is_fixnum(object *obj);

object* make_boolean(int value);
int boolean_init(void);
int is_boolean(object *obj);
int is_true(object *obj);
int is_false(object *obj);

object* make_character(int val);
int is_character(object *obj);

object* make_string(char *str);
int is_string(object *obj);

object* make_empty_list();
int is_empty_list(object *obj);
int empty_list_init();

int is_pair(object *obj);
object* cons(object *car, object *cdr);
object* car(object *pair);
object* cdr(object *pair);
int set_car(object *pair, object *car);
int set_cdr(object *pair, object *cdr);

#endif

