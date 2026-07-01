#include "rbtree.h"
#include "common.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct rb_tree_s {
  int8_t color;
  rb_tree_s *p;
  rb_tree_s *left;
  rb_tree_s *right;
  long key;
} rb_tree_s;

static rb_tree_s nil = {
    .p = NULL, .left = NULL, .right = NULL, .key = 0, .color = BLACK};
static rb_tree_s *EMPTY_TREE_NODE = &nil;

rb_tree_s *new_rb_node(long key) {
  rb_tree_s *new_tree = malloc(sizeof(rb_tree_s));
  if (new_tree) {
    new_tree->color = BLACK;
    new_tree->p = EMPTY_TREE_NODE;
    new_tree->left = EMPTY_TREE_NODE;
    new_tree->right = EMPTY_TREE_NODE;
    new_tree->key = key;
  }
  return new_tree;
}

bool rb_is_empty(rb_tree_s *node) {
  return (node == NULL || node == EMPTY_TREE_NODE);
}

void rb_destroy(void *node) {
  if (!rb_is_empty(node)) {
    rb_destroy(((rb_tree_s *)node)->right);
    rb_destroy(((rb_tree_s *)node)->left);
    SafeRelease(node);
  }
}

void rb_set_color(rb_tree_s *node, int8_t color) { node->color = color; }

void rb_set_key(rb_tree_s *node, long key) { node->key = key; }

void rb_set_p(rb_tree_s *node, rb_tree_s *parent) { node->p = parent; }

void rb_set_left(rb_tree_s *node, rb_tree_s *left) { node->left = left; }

void rb_set_right(rb_tree_s *node, rb_tree_s *right) { node->right = right; }

long rb_key(rb_tree_s *node) { return node->key; }

int8_t rb_color(rb_tree_s *node) { return node->color; }

rb_tree_s *rb_parent(rb_tree_s *node) {
  if (node != NULL) {
    return node->p;
  }
  return NULL;
}

rb_tree_s *rb_left(rb_tree_s *node) {
  if (node != NULL) {
    return node->left;
  }
  return NULL;
}

rb_tree_s *rb_right(rb_tree_s *node) {
  if (node != NULL) {
    return node->right;
  }
  return NULL;
}

rb_tree_s *rb_search(rb_tree_s *node, long target, int comparator(long, long)) {
  if (rb_is_empty(node) || comparator(target, node->key) == 0) {
    // printf("rb_is_empty %d and comparator %d\n", rb_is_empty(node));
    // if (!rb_is_empty(node)) {
    //   printf("and comparator %d\n", comparator(target, node->key) == 0);
    //   printf("target %ld node->key %ld\n", target, node->key);
    // }
    return node;
  }
  if (comparator(target, node->key) == -1) {
    return rb_search(node->left, target, comparator);
  } else {
    return rb_search(node->right, target, comparator);
  }
}

rb_tree_s *rb_max(rb_tree_s *node) {
  while (!rb_is_empty(node->right)) {
    node = node->right;
  }
  return node;
}

rb_tree_s *rb_min(rb_tree_s *node) {
  while (!rb_is_empty(node->left)) {
    node = node->left;
  }
  return node;
}

rb_tree_s *rb_successor(rb_tree_s *node) {
  if (!rb_is_empty(node->right)) {
    return rb_min(node->right);
  }
  rb_tree_s *buf = node->p;
  while (!rb_is_empty(buf)) {
    if (node == buf->right) {
      node = buf;
      buf = buf->p;
      continue;
    }
    break;
  }
  return buf;
}

rb_tree_s *rb_predecessor(rb_tree_s *node) {
  if (!rb_is_empty(node->left)) {
    return rb_max(node->left);
  }
  rb_tree_s *buf = node->p;
  while (!rb_is_empty(buf)) {
    if (node == buf->left) {
      node = buf;
      buf = buf->p;
      continue;
    }
    break;
  }
  return buf;
}

static void rb_left_rotate(rb_tree_s **root, rb_tree_s *x) {
  rb_tree_s *y = x->right;
  x->right = y->left;
  if (!rb_is_empty(y->left)) {
    y->left->p = x;
  }
  y->p = x->p;
  if (rb_is_empty(x->p)) {
    *root = y;
  } else if (x == x->p->left) {
    x->p->left = y;
  } else
    x->p->right = y;
  y->left = x;
  x->p = y;
}

static void rb_right_rotate(rb_tree_s **root, rb_tree_s *x) {
  rb_tree_s *y = x->left;
  x->left = y->right;
  if (!rb_is_empty(y->right)) {
    y->right->p = x;
  }
  y->p = x->p;
  if (rb_is_empty(x->p)) {
    *root = y;
  } else if (x == x->p->right) {
    x->p->right = y;
  } else
    x->p->left = y;
  y->right = x;
  x->p = y;
}

static void rb_insert_fix(rb_tree_s **root, rb_tree_s *node) {
  rb_tree_s *y;
  while (node->p->color == RED) {
    if (node->p == node->p->p->left) {
      y = node->p->p->right;
      if (y->color == RED) {
        node->p->color = BLACK;
        y->color = BLACK;
        node->p->p->color = RED;
        node = node->p->p;
      } else {
        if (node == node->p->right) {
          node = node->p;
          rb_left_rotate(root, node);
        }
        node->p->color = BLACK;
        node->p->p->color = RED;
        rb_right_rotate(root, node->p->p);
      }
    } else {
      y = node->p->p->left;
      if (y->color == RED) {
        node->p->color = BLACK;
        y->color = BLACK;
        node->p->p->color = RED;
        node = node->p->p;
      } else {
        if (node == node->p->left) {
          node = node->p;
          rb_right_rotate(root, node);
        }
        node->p->color = BLACK;
        node->p->p->color = RED;
        rb_left_rotate(root, node->p->p);
      }
    }
  }
  (*root)->color = BLACK;
}

bool rb_insert(rb_tree_s **root, long v, int comparator(long, long)) {
  rb_tree_s *z = new_rb_node(v);
  if (z == NULL)
    return false;
  if (root != NULL) {
    if (*root == NULL) {
      *root = z;
      return true;
    }
    rb_tree_s *x = *root;
    rb_tree_s *y = EMPTY_TREE_NODE;
    while (!rb_is_empty(x)) {
      y = x;
      if (comparator(z->key, x->key) == -1)
        x = x->left;
      else
        x = x->right;
    }
    z->p = y;
    if (rb_is_empty(y)) {
      *root = z;
    } else if (comparator(z->key, y->key) == -1)
      y->left = z;
    else
      y->right = z;
    z->left = EMPTY_TREE_NODE;
    z->right = EMPTY_TREE_NODE;
    z->color = RED;
    rb_insert_fix(root, z);
    return true;
  }
  return false;
}

static void rb_transplant(rb_tree_s **root, rb_tree_s *u, rb_tree_s *v) {
  if (rb_is_empty(u->p)) {
    *root = v;
  } else if (u == u->p->left) {
    u->p->left = v;
  } else
    u->p->right = v;

  if (v != EMPTY_TREE_NODE) {
    v->p = u->p;
  }
}

static void rb_delete_fix(rb_tree_s **root, rb_tree_s *x) {
  while (x != *root && x->color == BLACK) {
    rb_tree_s *w;
    if (x == x->p->left) {
      w = x->p->right;
      if (w->color == RED) {
        w->color = BLACK;
        x->p->color = RED;
        rb_left_rotate(root, x->p);
        w = x->p->right;
      }
      if (w->left->color == BLACK && w->right->color == BLACK) {
        w->color = RED;
        x = x->p;
      } else {
        if (w->right->color == BLACK) {
          w->left->color = BLACK;
          w->color = RED;
          rb_right_rotate(root, w);
          w = x->p->right;
        }
        w->color = x->p->color;
        x->p->color = BLACK;
        w->right->color = BLACK;
        rb_left_rotate(root, x->p);
        x = *root;
      }
    } else {
      w = x->p->left;
      if (w->color == RED) {
        w->color = BLACK;
        x->p->color = RED;
        rb_right_rotate(root, x->p);
        w = x->p->left;
      }
      if (w->right->color == BLACK && w->left->color == BLACK) {
        w->color = RED;
        x = x->p;
      } else {
        if (w->left->color == BLACK) {
          w->right->color = BLACK;
          w->color = RED;
          rb_left_rotate(root, w);
          w = x->p->left;
        }
        w->color = x->p->color;
        x->p->color = BLACK;
        w->left->color = BLACK;
        rb_right_rotate(root, x->p);
        x = *root;
      }
    }
  }
  x->color = BLACK;
}

bool rb_delete(rb_tree_s **root, rb_tree_s *z) {
  if (!rb_is_empty(*root) && !rb_is_empty(z)) {
    rb_tree_s *x;
    rb_tree_s *y = z;
    int8_t y_original_color = y->color;
    if (rb_is_empty(z->left)) {
      x = z->right;
      rb_transplant(root, z, z->right);
    } else if (rb_is_empty(z->right)) {
      x = z->left;
      rb_transplant(root, z, z->left);
    } else {
      y = rb_min(z->right);
      y_original_color = y->color;
      x = y->right;
      if (y != z->right) {
        rb_transplant(root, y, y->right);
        y->right = z->right;
        y->right->p = y;
      } else {
        if (x != EMPTY_TREE_NODE) {
          x->p = y;
        }
      };
      rb_transplant(root, z, y);
      y->left = z->left;
      y->left->p = y;
      y->color = z->color;
    }
    if (y_original_color == BLACK) {
      rb_delete_fix(root, x);
    }
    SafeRelease(z);
    return true;
  }
  return false;
}

rb_tree_s *rb_start(rb_tree_s *root) { return rb_min(root); }

bool rb_has_next(rb_tree_s *node) {
  if (!rb_is_empty(rb_successor(node))) {
    return true;
  }
  return false;
}

bool rb_has_prev(rb_tree_s *node) {
  if (!rb_is_empty(rb_predecessor(node))) {
    return true;
  }
  return false;
}

rb_tree_s *rb_prev(rb_tree_s *node) { return rb_predecessor(node); }

rb_tree_s *rb_next(rb_tree_s *node) { return rb_successor(node); }

rb_tree_s *rb_end(rb_tree_s *root) { return rb_max(root); }