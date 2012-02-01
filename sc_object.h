#ifndef _SC_OBJECT_H_
#define _SC_OBJECT_H_

typedef char bool

typedef enum {
	FIXNUM
} object_type;

typedef struct object {
    object_type type;
    union {
        struct {
            long value;
        } fixnum;
    } data;
} object;

#endif
