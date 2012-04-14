#include <stdlib.h>
#include <stdio.h>
#include <setjmp.h>
#include <string.h>
#include "object.h"
#include "mem.h"
#include "cont.h"
#include "stack.h"
#include "gc.h"
#include "log.h"

extern long *g_stack_bottom;
static volatile object *escape_val;

static void fatal(char *msg) {
    fprintf(stderr, "[cont] %s\n", msg);
    exit(-1);
}

static cont* alloc_cont(void) {
    cont *c;
    c = sc_malloc(sizeof(cont));
    if (c == NULL) {
        fatal("not enough memory");
    }
    return c;
}
/*
static void dump_cont(cont *c) {
    fprintf(stderr, "stack=%p,stack size=%ld\n",
            cont_stack(c), cont_stacksize(c));
    fprintf(stderr, "gc root stack size=%d\n", cont_gcstack(c).size);
}
*/
static void save_stack(cont *c, long *pbos, long *ptos) {
    long n = pbos - ptos;
    long *stk;
    long i;

    stk = sc_malloc(sizeof(long) * n);
    if (stk == NULL) {
        fatal("not enough memory");
    }
    for (i = 0; i < n; i++) {
        stk[i] = pbos[-i];
    }
    cont_stack(c) = stk;
    cont_stacksize(c) = n;
}

static void save_gc_state(cont *c) {
    object **cap;

    cap = gc_stack_root_deepcopy();
    if (cap == NULL) {
        fatal("not enough memory");
    }
    cont_capture(c) = cap;
    if (gc_stack_root_copy(&cont_gcstack(c)) != 0) {
        fatal("failed to copy gc root stack");
    }
}

int is_cont(object *obj) {
    return obj != NULL && type(obj) == CONT;
}

/* save_cont returns NULL when control returns from longjmp */
object* save_cont(void) {
    cont *c = alloc_cont();
    int ret;
    volatile long stack_top;

    if ((ret = setjmp(cont_ctx(c))) == 0) {
        save_stack(c, g_stack_bottom, (long*)&stack_top);
        save_gc_state(c);

        object *obj = alloc_object();
        obj_cont(obj) = c;
        type(obj) = CONT;
        return obj;
    } else {
        /* continuation return */
        escape_val = (object*)ret;
        return NULL;
    }
}

/* restore_cont never returns, control is passed to save_cont */
void internal_restore_cont(cont *c, object *val, int once_more) {
    volatile long padding[32];
    long tos;
    int i, n;
    long *stk;
    
    /* suppress unused warning */
    padding[0] = 12;
    padding[2] = padding[0] + 2;

    /* make sure there's enough room on the stack */
    if (g_stack_bottom - cont_stacksize(c) < &tos) {
        internal_restore_cont(c, val, 1);
    }
    if (once_more) {
        internal_restore_cont(c, val, 0);
    }

    /* restore gc root stack */
    gc_stack_root_swap(&cont_gcstack(c));

    /* restore stack */
    n = cont_stacksize(c);
    stk = cont_stack(c);
    for (i = 0; i < n; i++) {
        g_stack_bottom[-i] = stk[i];
    }
    longjmp(cont_ctx(c), (int)val);
}

object* get_escape_val(void) {
    return (object*)escape_val;
}

void cont_free(object *obj) {
    cont *c = obj_cont(obj);
    sc_free(cont_capture(c));
    sc_free(cont_gcstack(c).elems);
    sc_free(cont_stack(c));
    sc_free(c);
    obj_cont(obj) = NULL;
}

