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

/* Глобальный NIL-узел (лист) с корректным p (указывает на себя) */
static rb_tree_s nil = {BLACK, &nil, NULL, NULL, 0};
static rb_tree_s *NIL = &nil;

/* Конструктор узла — создаёт чёрный узел (как в вашем исходном коде) */
rb_tree_s *new_rb_node(long key) {
  rb_tree_s *node = malloc(sizeof(rb_tree_s));
  if (node) {
    node->color = BLACK;
    node->p = NIL;
    node->left = NIL;
    node->right = NIL;
    node->key = key;
  }
  return node;
}

bool rb_is_empty(rb_tree_s *node) { return (node == NULL || node == NIL); }

void rb_destroy(void *node) {
  if (!rb_is_empty(node)) {
    rb_destroy(((rb_tree_s *)node)->left);
    rb_destroy(((rb_tree_s *)node)->right);
    SafeRelease(node);
  }
}

/* Сеттеры и геттеры с защитой от изменения NIL */
void rb_set_color(rb_tree_s *node, int8_t color) {
  if (node && node != NIL)
    node->color = color;
}
void rb_set_key(rb_tree_s *node, long key) {
  if (node && node != NIL)
    node->key = key;
}
void rb_set_p(rb_tree_s *node, rb_tree_s *parent) {
  if (node && node != NIL)
    node->p = parent;
}
void rb_set_left(rb_tree_s *node, rb_tree_s *left) {
  if (node && node != NIL)
    node->left = left;
}
void rb_set_right(rb_tree_s *node, rb_tree_s *right) {
  if (node && node != NIL)
    node->right = right;
}

long rb_key(rb_tree_s *node) { return (node && node != NIL) ? node->key : 0; }
int8_t rb_color(rb_tree_s *node) {
  return (node && node != NIL) ? node->color : BLACK;
}
rb_tree_s *rb_parent(rb_tree_s *node) {
  return (node && node != NIL) ? node->p : NULL;
}
rb_tree_s *rb_left(rb_tree_s *node) {
  return (node && node != NIL) ? node->left : NULL;
}
rb_tree_s *rb_right(rb_tree_s *node) {
  return (node && node != NIL) ? node->right : NULL;
}

/* Поиск */
rb_tree_s *rb_search(rb_tree_s *node, long target, int comparator(long, long)) {
  while (!rb_is_empty(node)) {
    int cmp = comparator(target, node->key);
    if (cmp == 0)
      return node;
    node = (cmp < 0) ? node->left : node->right;
  }
  return NIL;
}

/* Минимум и максимум */
rb_tree_s *rb_min(rb_tree_s *node) {
  if (rb_is_empty(node))
    return NIL;
  while (!rb_is_empty(node->left))
    node = node->left;
  return node;
}

rb_tree_s *rb_max(rb_tree_s *node) {
  if (rb_is_empty(node))
    return NIL;
  while (!rb_is_empty(node->right))
    node = node->right;
  return node;
}

/* Следующий и предыдущий */
rb_tree_s *rb_successor(rb_tree_s *node) {
  if (rb_is_empty(node))
    return NIL;
  if (!rb_is_empty(node->right))
    return rb_min(node->right);
  rb_tree_s *p = node->p;
  while (!rb_is_empty(p) && node == p->right) {
    node = p;
    p = p->p;
  }
  return p;
}

rb_tree_s *rb_predecessor(rb_tree_s *node) {
  if (rb_is_empty(node))
    return NIL;
  if (!rb_is_empty(node->left))
    return rb_max(node->left);
  rb_tree_s *p = node->p;
  while (!rb_is_empty(p) && node == p->left) {
    node = p;
    p = p->p;
  }
  return p;
}

/* Повороты */
static void rb_left_rotate(rb_tree_s **root, rb_tree_s *x) {
  rb_tree_s *y = x->right;
  x->right = y->left;
  if (!rb_is_empty(y->left))
    y->left->p = x;
  y->p = x->p;
  if (rb_is_empty(x->p))
    *root = y;
  else if (x == x->p->left)
    x->p->left = y;
  else
    x->p->right = y;
  y->left = x;
  x->p = y;
}

static void rb_right_rotate(rb_tree_s **root, rb_tree_s *x) {
  rb_tree_s *y = x->left;
  x->left = y->right;
  if (!rb_is_empty(y->right))
    y->right->p = x;
  y->p = x->p;
  if (rb_is_empty(x->p))
    *root = y;
  else if (x == x->p->right)
    x->p->right = y;
  else
    x->p->left = y;
  y->right = x;
  x->p = y;
}

/* Балансировка после вставки */
static void rb_insert_fix(rb_tree_s **root, rb_tree_s *z) {
  while (z->p->color == RED) {
    if (z->p == z->p->p->left) {
      rb_tree_s *y = z->p->p->right;
      if (y->color == RED) {
        z->p->color = BLACK;
        y->color = BLACK;
        z->p->p->color = RED;
        z = z->p->p;
      } else {
        if (z == z->p->right) {
          z = z->p;
          rb_left_rotate(root, z);
        }
        z->p->color = BLACK;
        z->p->p->color = RED;
        rb_right_rotate(root, z->p->p);
      }
    } else {
      rb_tree_s *y = z->p->p->left;
      if (y->color == RED) {
        z->p->color = BLACK;
        y->color = BLACK;
        z->p->p->color = RED;
        z = z->p->p;
      } else {
        if (z == z->p->left) {
          z = z->p;
          rb_right_rotate(root, z);
        }
        z->p->color = BLACK;
        z->p->p->color = RED;
        rb_left_rotate(root, z->p->p);
      }
    }
  }
  (*root)->color = BLACK;
}

/* Вставка */
bool rb_insert(rb_tree_s **root, long v, int comparator(long, long)) {
  if (root == NULL)
    return false;

  if (*root != NULL) {
    rb_tree_s *existing = rb_search(*root, v, comparator);
    if (!rb_is_empty(existing))
      return false;
  }

  rb_tree_s *z = new_rb_node(v);
  if (z == NULL)
    return false;

  rb_tree_s *y = NIL;
  rb_tree_s *x = *root;
  while (!rb_is_empty(x)) {
    y = x;
    if (comparator(z->key, x->key) < 0)
      x = x->left;
    else
      x = x->right;
  }
  z->p = y;
  if (rb_is_empty(y))
    *root = z;
  else if (comparator(z->key, y->key) < 0)
    y->left = z;
  else
    y->right = z;

  z->left = NIL;
  z->right = NIL;
  z->color = RED;
  rb_insert_fix(root, z);
  return true;
}

/* Пересадка поддеревьев */
static void rb_transplant(rb_tree_s **root, rb_tree_s *u, rb_tree_s *v) {
  if (rb_is_empty(u->p))
    *root = v;
  else if (u == u->p->left)
    u->p->left = v;
  else
    u->p->right = v;
  v->p = u->p; // даже если v == NIL, это безопасно, т.к. NIL->p существует
}

/* Балансировка после удаления */
static void rb_delete_fix(rb_tree_s **root, rb_tree_s *x) {
  while (x != *root && x->color == BLACK) {
    if (x == x->p->left) {
      rb_tree_s *w = x->p->right;
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
      rb_tree_s *w = x->p->left;
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

/* Удаление узла */
bool rb_delete(rb_tree_s **root, rb_tree_s *z) {
  if (root == NULL || rb_is_empty(*root) || rb_is_empty(z))
    return false;

  rb_tree_s *y = z;
  rb_tree_s *x;
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
      x->p = y; // если x == NIL, NIL->p = y (корректно)
    }
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

/* Итераторы */
rb_tree_s *rb_start(rb_tree_s *root) { return rb_min(root); }
rb_tree_s *rb_end(rb_tree_s *root) { return rb_max(root); }
bool rb_has_next(rb_tree_s *node) { return !rb_is_empty(rb_successor(node)); }
bool rb_has_prev(rb_tree_s *node) { return !rb_is_empty(rb_predecessor(node)); }
rb_tree_s *rb_next(rb_tree_s *node) { return rb_successor(node); }
rb_tree_s *rb_prev(rb_tree_s *node) { return rb_predecessor(node); }