#include <stdlib.h>
#include "object.h"
#include "mem.h"
#include "config.h"

typedef enum color_type {
    RED,
    BLACK
} color_type;

/**
 * var must be a symbol, a total order is maintained using
 * simple pointer comparison. Thus no hash needed.
 */
typedef struct rbnode {
    object *var;
    object *val;
    rbtree left;
    rbtree right;
    color_type color;
} rbnode;

#define rbleft(p) ((p)->left)
#define rbright(p) ((p)->right)
#define rbcolor(p) ((p)->color)
#define rbvar(p) ((p)->var)
#define rbval(p) ((p)->val)

#define VAR_MIN (void*)0

static rbnode *null_node;

int env_frame_init(void) {
    null_node = sc_malloc(sizeof(rbnode));
    if (null_node == NULL) {
        return -1;
    }
    rbleft(null_node) = null_node;
    rbright(null_node) = null_node;
    rbcolor(null_node) = BLACK;
    rbvar(null_node) = VAR_MIN; /* dummy */
    rbval(null_node) = NULL;
    return 0;
}

void env_frame_dispose() {
    sc_free(null_node);
}

int is_env_frame(object *obj) {
    return obj != NULL && type(obj) == ENV_FRAME;
}

object* make_env_frame(void) {
    object *p;
    rbtree t;

    t = sc_malloc(sizeof(rbnode));
    if (t == NULL) {
        return NULL;
    }
    rbvar(t) = VAR_MIN;
    rbval(t) = NULL;
    rbleft(t) = null_node;
    rbright(t) = null_node;
    rbcolor(t) = BLACK;

    p = alloc_object();
    type(p) = ENV_FRAME;
    obj_rbtv(p) = t;
    return p;
}

static object* rb_find(rbtree t, object *var) {
    if (t == null_node) {
        return NULL;
    }
    if (var < rbvar(t)) {
        return rb_find(rbleft(t), var);
    } else if (var > rbvar(t)) {
        return rb_find(rbright(t), var);
    } else {
        return rbval(t);
    }
}

/**
 * return NULL if not found
 */
object* env_frame_find(object *frame, object *var) {
    rbtree t = obj_rbtv(frame);
    return rb_find(t, var);
}

static void rb_free(rbtree t) {
    if (t != null_node) {
        rb_free(rbleft(t));
        rb_free(rbright(t));
        sc_free(t);
    }
}

void env_frame_free(object *obj) {
    rbtree t = obj_rbtv(obj);
    rb_free(t);
}

/**
 * This function can be called only if k2 has a left child.
 * Perform a rotate between a node(k2) and its left child.
 * Return new root.
 */
static position single_rotate_with_left(position k2) {
    position k1;

    k1 = rbleft(k2);
    rbleft(k2) = rbright(k1);
    rbright(k1) = k2;
    
    return k1;
}

/**
 * This function can be called only if k1 has a right child.
 * Perform a rotate between a node(k1) and its right child.
 * Return new root.
 */
static position single_rotate_with_right(position k1) {
    position k2;

    k2 = rbright(k1);
    rbright(k1) = rbleft(k2);
    rbleft(k2) = k1;

    return k2;
}

/**
 * Perform a rotation at node X
 */
static position rotate(position parent, object *var) {
    if (var < rbvar(parent)) {
        rbleft(parent) = var < rbvar(rbleft(parent)) ?
            single_rotate_with_left(rbleft(parent)) :
            single_rotate_with_right(rbleft(parent));
        return rbleft(parent);
    } else {
        rbright(parent) = var < rbvar(rbright(parent)) ?
            single_rotate_with_left(rbright(parent)) :
            single_rotate_with_right(rbright(parent));
        return rbright(parent);
    }
}

static void handle_reorient(object *var, rbtree t, position *x,
                            position *p, position *gp, position *ggp) {
    /* flip color */
    rbcolor(*x) = RED;
    rbcolor(rbleft(*x)) = BLACK;
    rbcolor(rbright(*x)) = BLACK;

    if (rbcolor(*p) == RED) {
        /* need rotate */
        rbcolor(*gp) = RED;
        if ((var < rbvar(*gp)) != (var < rbvar(*p))) {
            *p = rotate(*gp, var); /* start double rotate */
        }
        *x = rotate(*ggp, var);
        rbcolor(*x) = BLACK;
    }
    rbcolor(rbright(t)) = BLACK; /* make root black */
}

static int rb_change(rbtree t, object *var, object *val) {
    if (t == null_node) {
        return -1;
    }
    if (var < rbvar(t)) {
        return rb_change(rbleft(t), var, val);
    } else if (var > rbvar(t)) {
        return rb_change(rbright(t), var, val);
    } else {
        rbval(t) = val;
        return 0;
    }
}

int env_frame_change(object *frame, object *var, object *val) {
    rbtree t = obj_rbtv(frame);
    return rb_change(t, var, val);
}

#ifdef VERIFY_RB_TREE
static void rb_verify(rbtree tree);
#endif

int env_frame_insert(object *frame, object *var, object *val) {
    position x, p, gp, ggp;
    rbtree t = obj_rbtv(frame);

    x = p = gp = t;
    rbvar(null_node) = var;
    while (rbvar(x) != var) {
        ggp = gp;
        gp = p;
        p = x;
        if (var < rbvar(x)) {
            x = rbleft(x);
        } else {
            x = rbright(x);
        }
        if (rbcolor(rbleft(x)) == RED &&
            rbcolor(rbright(x)) == RED) {
            handle_reorient(var, t, &x, &p, &gp, &ggp);
        }
    }

    if (x != null_node) {
        /* change existing node */
        rbval(x) = val;
    } else if (x == null_node) {
        x = sc_malloc(sizeof(rbnode));
        if (x == NULL) {
            return -2;
        }
        rbvar(x) = var;
        rbval(x) = val;
        rbleft(x) = null_node;
        rbright(x) = null_node;
        if (var < rbvar(p)) {
            rbleft(p) = x;
        } else {
            rbright(p) = x;
        }
        /* color it red; maybe rotate */
        handle_reorient(var, t, &x, &p, &gp, &ggp);
    }

#ifdef VERIFY_RB_TREE
    rb_verify(t);
#endif
    return 0;
}

void rb_walk(rbtree t, env_frame_walk_fn walker) {
    if (t == null_node) {
        return;
    }

    walker(rbvar(t), rbval(t));
    rb_walk(rbleft(t), walker);
    rb_walk(rbright(t), walker);
}

void env_frame_walk(object *frame, env_frame_walk_fn walker) {
    rbtree t = obj_rbtv(frame);
    rb_walk(rbright(t), walker);
}

/* rbtree verification functions */
#ifdef VERIFY_RB_TREE
static int rb_verify_node(rbtree t) {
    rbtree left, right;

    if (t == null_node) {
        return 1;
    }
    
    left = rbleft(t);
    right = rbright(t);
    if (rbcolor(t) == RED &&
        (rbcolor(left) != BLACK || rbcolor(right) != BLACK)) {
        return 0;
    }
    return rb_verify_node(left) && rb_verify_node(right);
}

static void rb_verify_black_node(rbtree t, int cnt, int *n) {
    if (t == null_node) {
        if (*n == -1) {
            *n = cnt;
            return;
        } else if (*n == cnt) {
            return;
        } else {
            fprintf(stderr,
                    "not every path from a node to a leaf \
                    contains the same number of black nodes\n");
            exit(-1);
        }
    }
    if (rbcolor(t) == BLACK) {
        cnt++;
    }
    rb_verify_black_node(rbleft(t), cnt, n);
    rb_verify_black_node(rbright(t), cnt, n);
}

static void rb_verify(rbtree tree) {
    rbtree t = rbright(tree);
    int n = -1;

    if (rbcolor(t) != BLACK) {
        fprintf(stderr, "root must be black\n");
        return;
    }
    if (rbcolor(null_node) != BLACK) {
        fprintf(stderr, "leaves must be black\n");
        return;
    }
    if (!rb_verify_node(t)) {
        fprintf(stderr, "children of red node must be black\n");
        return;
    }
    rb_verify_black_node(t, 0, &n);
}
#endif  /* VERIFY_RB_TREE */

