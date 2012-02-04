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
    } data;
} object;

#define type(p) (p->type)
#define obj_cv(p) (p->data.character.value)
#define obj_bv(p) (p->data.boolean.value)
#define obj_nv(p) (p->data.fixnum.value)

extern object *g_true_val;
extern object *g_false_val;

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

#endif

