#ifndef _CONT_H_
#define _CONT_H_

#include "object.h"
#include "stack.h"
#include "setjmp.h"

typedef struct cont {
    jmp_buf ctx;
    long *stack;
    long stack_size;
    struct object **capture;
    struct stack gc_root_stack;
} cont;

#define cont_ctx(c) (c->ctx)
#define cont_stack(c) (c->stack)
#define cont_stacksize(c) (c->stack_size)
#define cont_gcstack(c) (c->gc_root_stack)
#define cont_capture(c) (c->capture)

#endif

