#ifndef _SC_OBJECT_H_
#define _SC_OBJECT_H_

typedef enum {
	FIXNUM,
    BOOLEAN,
} object_type;

typedef struct object {
    object_type type;
    union {
        struct {
            char value;
        } boolean;
        struct {
            long value;
        } fixnum;
    } data;
} object;

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

