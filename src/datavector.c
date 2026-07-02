#include "datavector.h"
#include "common.h"
#include <stdlib.h>
#include <string.h>

enum { DEFAULT_CAPACITY = 10 };

struct DataVector {
  StatData *data;
  size_t size;
  size_t capacity;
};

/* ---- Геттеры ---- */
StatData DataVectorGet(const DataVector *v, size_t i) { return v->data[i]; }

size_t DataVectorSize(const DataVector *v) { return v->size; }

const StatData *DataVectorGetConstData(const DataVector *v) { return v->data; }

StatData *DataVectorGetMutableData(DataVector *v) { return v->data; }

void DataVectorSet(DataVector *v, size_t index, const StatData *data) {
  if (v && index < v->size) {
    v->data[index] = *data;
  }
}

/* ---- Управление ---- */
DataVector *DataVectorCreate(size_t num) {
  DataVector *v = malloc(sizeof(DataVector));
  if (!v)
    return NULL;
  if (num < 1)
    num = DEFAULT_CAPACITY;
  v->data = malloc(sizeof(StatData) * num);
  if (!v->data) {
    free(v);
    return NULL;
  }
  v->size = 0;
  v->capacity = num;
  return v;
}

void DataVectorDestroy(DataVector *v) {
  if (!v)
    return;
  SafeRelease(v->data);
  SafeRelease(v);
}

/* ---- Модификация ---- */
int DataVectorPush(DataVector *v, const StatData *data) {
  if (v->size == v->capacity) {
    size_t new_cap = v->capacity * 2;
    StatData *new_data = realloc(v->data, new_cap * sizeof(StatData));
    if (!new_data)
      return -1;
    v->data = new_data;
    v->capacity = new_cap;
  }
  v->data[v->size] = *data;
  v->size++;
  return 0;
}

int DataVectorSetData(DataVector *v, StatData *data, size_t size) {
  if (!v || !data)
    return -1;
  SafeRelease(v->data);
  v->data = data;
  v->size = size;
  v->capacity = size;
  return 0;
}

int DataVectorPop(DataVector *v, StatData *out) {
  if (v->size == 0)
    return -1;
  if (v->size * 2 <= v->capacity && v->capacity > DEFAULT_CAPACITY) {
    size_t new_cap = v->capacity / 2;
    StatData *new_data = realloc(v->data, new_cap * sizeof(StatData));
    if (new_data) {
      v->data = new_data;
      v->capacity = new_cap;
    }
  }
  v->size--;
  if (out)
    *out = v->data[v->size];
  return 0;
}