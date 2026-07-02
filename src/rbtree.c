#include "rbtree.h"
#include "common.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct RbTree {
  int8_t color;
  RbTree *p;
  RbTree *left;
  RbTree *right;
  long key;
} RbTree;

/* Глобальный NIL-узел (лист) с корректным p (указывает на себя) */
static RbTree nil = {BLACK, &nil, NULL, NULL, 0};
static RbTree *NIL = &nil;

/* Конструктор узла — создаёт чёрный узел (как в вашем исходном коде) */
RbTree *NewRbNode(long key) {
  RbTree *node = malloc(sizeof(RbTree));
  if (node) {
    node->color = BLACK;
    node->p = NIL;
    node->left = NIL;
    node->right = NIL;
    node->key = key;
  }
  return node;
}

bool RbIsEmpty(RbTree *node) { return (node == NULL || node == NIL); }

void RbDestroy(void *node) {
  if (!RbIsEmpty(node)) {
    RbDestroy(((RbTree *)node)->left);
    RbDestroy(((RbTree *)node)->right);
    SafeRelease(node);
  }
}

/* Сеттеры и геттеры с защитой от изменения NIL */
void RbSetColor(RbTree *node, int8_t color) {
  if (node && node != NIL)
    node->color = color;
}
void RbSetKey(RbTree *node, long key) {
  if (node && node != NIL)
    node->key = key;
}
void RbSetP(RbTree *node, RbTree *parent) {
  if (node && node != NIL)
    node->p = parent;
}
void RbSetLeft(RbTree *node, RbTree *left) {
  if (node && node != NIL)
    node->left = left;
}
void RbSetRight(RbTree *node, RbTree *right) {
  if (node && node != NIL)
    node->right = right;
}

long RbKey(RbTree *node) { return (node && node != NIL) ? node->key : 0; }
int8_t RbColor(RbTree *node) {
  return (node && node != NIL) ? node->color : BLACK;
}
RbTree *RbParent(RbTree *node) {
  return (node && node != NIL) ? node->p : NULL;
}
RbTree *RbLeft(RbTree *node) {
  return (node && node != NIL) ? node->left : NULL;
}
RbTree *RbRight(RbTree *node) {
  return (node && node != NIL) ? node->right : NULL;
}

/* Поиск */
RbTree *RbSearch(RbTree *node, long target, int comparator(long, long)) {
  while (!RbIsEmpty(node)) {
    int cmp = comparator(target, node->key);
    if (cmp == 0)
      return node;
    node = (cmp < 0) ? node->left : node->right;
  }
  return NIL;
}

/* Минимум и максимум */
RbTree *RbMin(RbTree *node) {
  if (RbIsEmpty(node))
    return NIL;
  while (!RbIsEmpty(node->left))
    node = node->left;
  return node;
}

RbTree *RbMax(RbTree *node) {
  if (RbIsEmpty(node))
    return NIL;
  while (!RbIsEmpty(node->right))
    node = node->right;
  return node;
}

/* Следующий и предыдущий */
RbTree *RbSuccessor(RbTree *node) {
  if (RbIsEmpty(node))
    return NIL;
  if (!RbIsEmpty(node->right))
    return RbMin(node->right);
  RbTree *p = node->p;
  while (!RbIsEmpty(p) && node == p->right) {
    node = p;
    p = p->p;
  }
  return p;
}

RbTree *RbPredecessor(RbTree *node) {
  if (RbIsEmpty(node))
    return NIL;
  if (!RbIsEmpty(node->left))
    return RbMax(node->left);
  RbTree *p = node->p;
  while (!RbIsEmpty(p) && node == p->left) {
    node = p;
    p = p->p;
  }
  return p;
}

/* Повороты */
static void rb_left_rotate(RbTree **root, RbTree *x) {
  RbTree *y = x->right;
  x->right = y->left;
  if (!RbIsEmpty(y->left))
    y->left->p = x;
  y->p = x->p;
  if (RbIsEmpty(x->p))
    *root = y;
  else if (x == x->p->left)
    x->p->left = y;
  else
    x->p->right = y;
  y->left = x;
  x->p = y;
}

static void rb_right_rotate(RbTree **root, RbTree *x) {
  RbTree *y = x->left;
  x->left = y->right;
  if (!RbIsEmpty(y->right))
    y->right->p = x;
  y->p = x->p;
  if (RbIsEmpty(x->p))
    *root = y;
  else if (x == x->p->right)
    x->p->right = y;
  else
    x->p->left = y;
  y->right = x;
  x->p = y;
}

/* Балансировка после вставки */
static void rb_insert_fix(RbTree **root, RbTree *z) {
  while (z->p->color == RED) {
    if (z->p == z->p->p->left) {
      RbTree *y = z->p->p->right;
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
      RbTree *y = z->p->p->left;
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
bool RbInsert(RbTree **root, long v, int comparator(long, long)) {
  if (root == NULL)
    return false;

  if (*root != NULL) {
    RbTree *existing = RbSearch(*root, v, comparator);
    if (!RbIsEmpty(existing))
      return false;
  }

  RbTree *z = NewRbNode(v);
  if (z == NULL)
    return false;

  RbTree *y = NIL;
  RbTree *x = *root;
  while (!RbIsEmpty(x)) {
    y = x;
    if (comparator(z->key, x->key) < 0)
      x = x->left;
    else
      x = x->right;
  }
  z->p = y;
  if (RbIsEmpty(y))
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
static void rb_transplant(RbTree **root, RbTree *u, RbTree *v) {
  if (RbIsEmpty(u->p))
    *root = v;
  else if (u == u->p->left)
    u->p->left = v;
  else
    u->p->right = v;
  v->p = u->p; // даже если v == NIL, это безопасно, т.к. NIL->p существует
}

/* Балансировка после удаления */
static void rb_delete_fix(RbTree **root, RbTree *x) {
  while (x != *root && x->color == BLACK) {
    if (x == x->p->left) {
      RbTree *w = x->p->right;
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
      RbTree *w = x->p->left;
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
bool RbDelete(RbTree **root, RbTree *z) {
  if (root == NULL || RbIsEmpty(*root) || RbIsEmpty(z))
    return false;

  RbTree *y = z;
  RbTree *x;
  int8_t y_original_color = y->color;

  if (RbIsEmpty(z->left)) {
    x = z->right;
    rb_transplant(root, z, z->right);
  } else if (RbIsEmpty(z->right)) {
    x = z->left;
    rb_transplant(root, z, z->left);
  } else {
    y = RbMin(z->right);
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
RbTree *RbStart(RbTree *root) { return RbMin(root); }
RbTree *RbEnd(RbTree *root) { return RbMax(root); }
bool RbHasNext(RbTree *node) { return !RbIsEmpty(RbSuccessor(node)); }
bool RbHasPrev(RbTree *node) { return !RbIsEmpty(RbPredecessor(node)); }
RbTree *RbNext(RbTree *node) { return RbSuccessor(node); }
RbTree *RbPrev(RbTree *node) { return RbPredecessor(node); }