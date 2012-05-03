#ifndef _QUEUE_H_
#define _QUEUE_H_

typedef struct queue_node {
    object *elem;
    struct queue_node *next;
} queue_node;

typedef struct queue {
    queue_node **cache;
    int cache_size;
    queue_node *head;
    queue_node *rear;
} queue;

queue *queue_new(void);
void queue_dispose(queue *q);
int enqueue(queue *q, object *e);
object* dequeue(queue *q);
int queue_isempty(queue *q);

#endif
