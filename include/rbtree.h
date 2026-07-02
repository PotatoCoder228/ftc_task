#pragma once

#include "stdint.h"
#include <stdbool.h>

static const int8_t BLACK = 0;
static const int8_t RED = 1;

typedef void *(*Callback)(void *data);

typedef struct RbTree RbTree;

RbTree *NewRbNode(long key);

void RbDestroy(void *node);

bool RbIsEmpty(RbTree *node);

void RbSetColor(RbTree *node, int8_t color);

void RbSetKey(RbTree *node, long key);

void RbSetP(RbTree *node, RbTree *parent);

void RbSetLeft(RbTree *node, RbTree *left);

void RbSetRight(RbTree *node, RbTree *right);

long RbKey(RbTree *node);

int8_t RbColor(RbTree *node);

RbTree *RbParent(RbTree *node);

RbTree *RbLeft(RbTree *node);

RbTree *RbRight(RbTree *node);

RbTree *RbSearch(RbTree *node, long target, int comparator(long, long));

RbTree *RbMax(RbTree *node);

RbTree *RbMin(RbTree *node);

RbTree *RbSuccessor(RbTree *node);

RbTree *RbPredecessor(RbTree *node);

bool RbInsert(RbTree **root, long v, int comparator(long, long));

bool RbDelete(RbTree **root, RbTree *z);

RbTree *RbStart(RbTree *root);

bool RbHasNext(RbTree *node);

bool RbHasPrev(RbTree *node);

RbTree *RbPrev(RbTree *node);

RbTree *RbNext(RbTree *node);

RbTree *RbEnd(RbTree *root);