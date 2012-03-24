#ifndef _STACK_H_
#define _STACK_H_

#include "object.h"

typedef object** stack_elem;

typedef struct stack {
    stack_elem *elems;
    int capacity;
    int size;
} stack;

typedef void (*visitor_fn)(stack_elem elem);

stack* stack_new();
void stack_dispose(stack *s);
void stack_push(stack *s, stack_elem obj);
void stack_pop(stack *s);
void stack_for_each(stack *s, visitor_fn fn);
object **stack_deepcopy(stack *s);
int stack_copy(stack *this, stack *dst);
int stack_swap(stack *this, stack *src);

#endif

