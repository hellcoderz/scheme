#include "mem.h"
#include "log.h"
#include "queue.h"

#define QUEUE_CACHE_SIZE 1024

queue *queue_new(void) {
    queue *q;
    queue_node **cache;

    q = sc_malloc(sizeof(queue));
    if (q == NULL) {
        return NULL;
    }
    cache = sc_malloc(sizeof(queue_node*) * QUEUE_CACHE_SIZE);
    if (cache == NULL) {
        sc_free(q);
        return NULL;
    }

    q->cache = cache;
    q->cache_size = 0;
    q->head = q->rear = NULL;
    return q;
}

void queue_dispose(queue *q) {
    int i;

    if (q == NULL) {
        return;
    }
    for (i = 0; i < q->cache_size; i++) {
        sc_free(q->cache[i]);
    }
    sc_free(q->cache);
    sc_free(q);
}

int queue_isempty(queue *q) {
    if (q == NULL) {
        return 1;
    }
    return q->head == NULL;
}

int enqueue(queue *q, object *elem) {
    int sz;
    queue_node *p;

    if (q == NULL) {
        return 1;
    }

    sz = q->cache_size;
    if (sz > 0) {
        p = q->cache[--sz];
        q->cache_size = sz;
        /* sc_log("queue cache hit, size=%d\n", sz+1); */
    } else {
        p = sc_malloc(sizeof(queue_node));
        if (p == NULL) {
            return -1;
        }
    }

    p->elem = elem;
    p->next = NULL;
    if (q->rear != NULL) {
        q->rear->next = p;
    }
    q->rear = p;
    if (q->head == NULL) {
        q->head = p;
    }
    return 0;
}

object* dequeue(queue *q) {
    int sz;
    queue_node *p;
    object *obj = NULL;

    if (q == NULL) {
        return NULL;
    }

    if (!queue_isempty(q)) {
        p = q->head;
        q->head = q->head->next;
        if (q->head == NULL) {
            q->rear = NULL;
        }
        obj = p->elem;

        sz = q->cache_size;
        if (sz < QUEUE_CACHE_SIZE) {
            /* cache this node */
            q->cache[sz++] = p;
            q->cache_size = sz;
        } else {
            sc_free(p);
        }
    }
    return obj;
}

