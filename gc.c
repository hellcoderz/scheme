#include <stdio.h>
#include <string.h>
#include "object.h"
#include "gc.h"
#include "config.h"
#include "repl.h"
#include "stack.h"
#include "log.h"
#include "mem.h"
#include "sform.h"

static gc_heap heap;
static gc_list free_list, active_list;
static int heap_empty = 0;
static stack *stack_root;

#define ACTIVE  1
#define FREE    0

/* utility functions */
#define mark_active(p) \
    gc_mark(p) = ACTIVE
#define mark_free(p) \
    gc_mark(p) = FREE
#define is_active(p) \
    gc_mark(p) == ACTIVE
#define is_free(p) \
    gc_mark(p) == FREE

#ifdef GC_DEBUG_INFO
static void dump_object(object *obj) {
    if (obj == NULL) {
        return;
    }
    switch (type(obj)) {
        case FIXNUM:
            fprintf(stderr, "fixnum@%p<%ld>\n",
                    obj, obj_nv(obj));
            break;
        case FLONUM:
            fprintf(stderr, "flonum@%p<%.8g>\n",
                    obj, obj_rv(obj));
            break;
        case CHARACTER:
            fprintf(stderr, "character@%p<%c>\n",
                    obj, obj_cv(obj));
            break;
        case BOOLEAN:
            fprintf(stderr, "boolean@%p<%s>\n",
                    obj, is_false(obj) ? "false" : "true");
            break;
        case STRING:
            fprintf(stderr, "string@%p<%s>\n",
                    obj, obj_sv(obj));
            break;
        case SYMBOL:
            fprintf(stderr, "symbol@%p<%s>\n",
                    obj, obj_iv(obj));
            break;
        case PAIR:
            fprintf(stderr, "pair@%p<car=%p,cdr=%p>\n",
                    obj, car(obj), cdr(obj));
            break;
        case THE_EMPTY_LIST:
            fprintf(stderr, "empty list@%p\n", obj);
            break;
        case PRIMITIVE_PROC:
            fprintf(stderr, "primitive proc@%p<fn=%p>\n",
                    obj, obj_fv(obj));
            break;
        case COMPOUND_PROC:
            fprintf(stderr, "compound proc@%p<p=%p,b=%p,e=%p>\n",
                    obj, obj_lvp(obj), obj_lvb(obj), obj_lve(obj));
            break;
        case INPUT_PORT:
            fprintf(stderr, "input port@%p<file=%p>\n",
                    obj, obj_ipv(obj));
            break;
        case OUTPUT_PORT:
            fprintf(stderr, "output port@%p<file=%p>\n",
                    obj, obj_opv(obj));
            break;
        case VECTOR:
            fprintf(stderr, "vector@%p<buf=%p,size=%d>\n",
                    obj, obj_vav(obj), obj_vsv(obj));
            break;
        default:
            break;
    }
}
#endif

void dump_gc_summary(void) {
    fprintf(stderr, "---------------->gc summary<-----------------\n");
    fprintf(stderr, "heap: base=%p, total=%d, used=%d\n",
            heap.segments, heap.seg_total, heap.seg_used);
    fprintf(stderr, "# of objects in freelist=%d\n",
            free_list.size);
    fprintf(stderr, "# of objects in activelist=%d\n",
            active_list.size);
    fprintf(stderr, "---------------------------------------------\n");
}

/* internal heap management functions */
static int heap_init(int heap_size) {
   object *p;
   int size = (heap_size < DEFAULT_HEAP_SIZE) ?
                DEFAULT_HEAP_SIZE : heap_size;

   p = sc_malloc(size * SEGMENT_SIZE * sizeof(object));
   if (p == NULL) {
       return -1;
   }
   heap.segments = p;
   heap.seg_total = size;
   heap.seg_used = 0;

   return 0;
}

static gc_segment* seg_setup(object *seg) {
    /* call sc_free to free returned segment object */
    object *p = seg;
    object *q = seg + SEGMENT_SIZE - 1;
    gc_segment *ret;

    ret = sc_malloc(sizeof(gc_segment));
    if (ret == NULL) {
        return NULL;
    }

    for (; p < q; p++) {
        gc_chain(p) = p + 1;
    }
    gc_chain(p) = NULL;
    ret->start = seg;
    ret->end = p;

    return ret;
}

static gc_segment* heap_alloc(void) {
    object *seg = NULL;
    int i = heap.seg_used;

    if (heap_empty) {
        return NULL;
    }

    if (i < heap.seg_total) {
        seg = &heap.segments[i * SEGMENT_SIZE];
        heap.seg_used++;
    }
    if (seg == NULL) {
        heap_empty = 1;
        return NULL;
    }
    
    return seg_setup(seg);
}


/* free_list and active_list common functions */
#define INIT_GC_LIST(list) \
    gc_chain(&(list.head)) = NULL; \
    list.size = 0;

static void lists_init(void) {
    INIT_GC_LIST(free_list);
    INIT_GC_LIST(active_list);
}

static int extend_freelist(void) {
    object *head = &(free_list.head);
    gc_segment *seg;

    seg = heap_alloc();
    if (seg == NULL) {
        return -1;
    }
    gc_chain(seg->end) = gc_chain(head);
    gc_chain(head) = seg->start;
    sc_free(seg);
    free_list.size += SEGMENT_SIZE;
    return 0;
}

static int gc_list_is_empty(gc_list *list) {
    object *head = &(list->head);
    return gc_chain(head) == NULL;
}

static object* gc_list_remove_front(gc_list *list) {
    object *head = &(list->head);
    object *front = gc_chain(head);

    gc_chain(head) = gc_chain(front);
    list->size = list->size - 1;
    return front;
}

static void gc_list_insert_front(gc_list *list, object *obj) {
    object *head = &(list->head);

    gc_chain(obj) = gc_chain(head);
    gc_chain(head) = obj;
    list->size = list->size + 1;
}

#define list_is_empty(list) \
    gc_list_is_empty(&list)
#define list_remove_front(list) \
    gc_list_remove_front(&list)
#define list_insert_front(list, obj) \
    gc_list_insert_front(&list, obj)

/* object allocation functions */
static object* gc_safe_alloc(void) {
    object *obj;

    if (list_is_empty(free_list)) {
        return NULL;
    }
    obj = list_remove_front(free_list);
    memset(obj, 0, sizeof(object));
    list_insert_front(active_list, obj);
    return obj;
}

object* gc_alloc(void) {
    int n = free_list.size;
    object *obj;

    if (n == GC_THRESHOLD) {
        gc();
        n = free_list.size;
        if (n <= EXTEND_THRESHOLD) {
            extend_freelist();
        }
    }

    obj = gc_safe_alloc();
    return obj;
}

static void gc_free(object *obj) {
    if (obj == NULL) {
        sc_log("%s", "gc_free got NULL argument");
        return;
    }

#ifdef GC_DEBUG_INFO
    dump_object(obj);
#endif

    list_insert_front(free_list, obj);
    /* do object specific free */
    switch (type(obj)) {
        case STRING:
            string_free(obj);
            break;
        case SYMBOL:
            symbol_free(obj);
            break;
        case INPUT_PORT:
        case OUTPUT_PORT:
            port_free(obj);
            break;
        case VECTOR:
            vector_free(obj);
            break;
        default:
            break;
    }
}

/* garbage collection functions */
static void mark_object(object *obj) {
tailcall:
    if (obj == NULL) {
        return;
    }
    
    /* skip marked objects, environments will recur */
    if (is_active(obj)) {
        return;
    }

    mark_active(obj);
    /* pairs, vector and compound procedures have nested objects */
    if (is_pair(obj)) {
        object *car_obj, *cdr_obj;
        car_obj = car(obj);
        cdr_obj = cdr(obj);
        mark_object(car_obj);
        obj = cdr_obj;
        goto tailcall;
    }
    if (is_compound_proc(obj)) {
        object *params = obj_lvp(obj);
        object *body = obj_lvb(obj);
        object *env = obj_lve(obj);
        mark_object(params);
        mark_object(body);
        obj = env;
        goto tailcall;
    }
    if (is_vector(obj)) {
        int i, len;
        object **array;
        len = obj_vsv(obj);
        array = obj_vav(obj);
        for (i = 0; i < len; i++) {
            mark_object(array[i]);
        }
    }
}

static void mark_stack_root(stack_elem elem) {
    mark_object(*(object**)elem);
}

static void mark_sform(void) {
    mark_active(get_quote_symbol());
    mark_active(get_set_symbol());
    mark_active(get_define_symbol());
    mark_active(get_nrv_symbol());
    mark_active(get_if_symbol());
    mark_active(get_lambda_symbol());
    mark_active(get_begin_symbol());
    mark_active(get_cond_symbol());
    mark_active(get_else_symbol());
    mark_active(get_let_symbol());
    mark_active(get_and_symbol());
    mark_active(get_or_symbol());
}

static void mark(void) {
    object *env;

    /* special forms can have no references,
     * make sure they are marked */
    mark_sform();

    /* mark all objects reachable from global environment */
    env = get_repl_env();
    mark_object(env);

    /* c funtion local objects, they must be protected
     * during GC */
    stack_for_each(stack_root, mark_stack_root);
}

static void sweep(void) {
    /* scan active list, free unmarked objects and mark active
     * objects as free for next GC. */
    object *head = &(active_list.head);
    object *prev, *curr, *next;
    int count = 0, free_count = 0;

    prev = head;
    curr = gc_chain(head);
    while (curr != NULL) {
        count++;
        if (is_active(curr)) {
            mark_free(curr);
            prev = curr;
            curr = gc_chain(curr);
        } else {
            free_count++;
            next = gc_chain(curr);
            gc_chain(prev) = next;
            active_list.size--;
            gc_free(curr);
            curr = next;
        }
    }

#ifdef GC_DEBUG_INFO
    fprintf(stderr, "scaned=%d, sweeped=%d\n", count, free_count);
#endif
}


void gc(void) {
#ifdef GC_DEBUG_INFO
    dump_gc_summary();
#endif

    mark();
    sweep();

#ifdef GC_DEBUG_INFO
    dump_gc_summary();
#endif
}

/* initialization functions */
int gc_init(int heap_size) {
    /* heap_size: number of segments */
    int ret;

    ret = heap_init(heap_size);
    if (ret != 0) {
        return ret;
    }

    lists_init();
    extend_freelist();

    stack_root = stack_new();

    return 0;
}

void gc_finalize(void) {
    stack_dispose(stack_root);
    sc_free(heap.segments);
    heap.segments = NULL;
}

/* stack root management functions */
void gc_stack_root_push(object **obj) {
    stack_elem elem = (stack_elem)obj;
    stack_push(stack_root, elem);
}

void gc_stack_root_pop() {
    stack_pop(stack_root);
}


