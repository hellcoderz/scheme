#ifndef _SC_OBJECT_H_
#define _SC_OBJECT_H_

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
#define obj_c(p) (p->data.character)
#define obj_b(p) (p->data.boolean)
#define obj_n(p) (p->data.fixnum)

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

#endif

