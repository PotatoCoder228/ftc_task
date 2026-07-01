#pragma once

#include "stdint.h"
#include <stdbool.h>

static const int8_t BLACK = 0;
static const int8_t RED = 1;

typedef void *(*callback)(void *data);

typedef struct rb_tree_s rb_tree_s;

rb_tree_s *new_rb_node(long key);

void rb_destroy(void *node);

bool rb_is_empty(rb_tree_s *node);

void rb_set_color(rb_tree_s *node, int8_t color);

void rb_set_key(rb_tree_s *node, long key);

void rb_set_p(rb_tree_s *node, rb_tree_s *parent);

void rb_set_left(rb_tree_s *node, rb_tree_s *left);

void rb_set_right(rb_tree_s *node, rb_tree_s *right);

long rb_key(rb_tree_s *node);

int8_t rb_color(rb_tree_s *node);

rb_tree_s *rb_parent(rb_tree_s *node);

rb_tree_s *rb_left(rb_tree_s *node);

rb_tree_s *rb_right(rb_tree_s *node);

rb_tree_s *rb_search(rb_tree_s *node, long target, int comparator(long, long));

rb_tree_s *rb_max(rb_tree_s *node);

rb_tree_s *rb_min(rb_tree_s *node);

rb_tree_s *rb_successor(rb_tree_s *node);

rb_tree_s *rb_predecessor(rb_tree_s *node);

bool rb_insert(rb_tree_s **root, long v, int comparator(long, long));

bool rb_delete(rb_tree_s **root, rb_tree_s *z);

rb_tree_s *rb_start(rb_tree_s *root);

bool rb_has_next(rb_tree_s *node);

bool rb_has_prev(rb_tree_s *node);

rb_tree_s *rb_prev(rb_tree_s *node);

rb_tree_s *rb_next(rb_tree_s *node);

rb_tree_s *rb_end(rb_tree_s *root);