#include "dumper.h"
#include "common.h"
#include "rbtree.h"
#include <assert.h>
#include <errno.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum Consts { DEFAULT_DATA_VECTOR_SIZE = 10, MAX_DATA_SIZE = 100000 };

// struct StatData {
//   long id;
//   int count;
//   float cost;
//   unsigned int primary : 1;
//   unsigned int mode : 3;
// };

struct DataVector {
  StatData *data;
  size_t size;
  size_t capacity;
};

StatData DataVectorGet(const DataVector *v, const size_t i) {
  return v->data[i];
}
size_t DataVectorSize(DataVector *v) { return v->size; }

int LongCompareOf(long v1, long v2) {
  if (v1 > v2)
    return 1;
  else if (v1 < v2)
    return -1;
  else
    return 0;
}
int CostCompareOf(const void *v1, const void *v2) {
  const StatData *a = (const StatData *)v1;
  const StatData *b = (const StatData *)v2;
  return (a->cost > b->cost) - (a->cost < b->cost);
}

DataVector *DataVectorCreate(size_t num) {
  DataVector *vector = malloc(sizeof(DataVector));

  if (vector) {
    if (num < 1)
      num = DEFAULT_DATA_VECTOR_SIZE;
    StatData *data = malloc(sizeof(StatData) * num);
    if (!data) {
      SafeRelease(vector);
      return NULL;
    }
    vector->data = data;
    vector->size = 0;
    vector->capacity = num;
    return vector;
  }
  return NULL;
}

int DataVectorPush(DataVector *vector, StatData *data) {
  if (vector->size == vector->capacity) {
    size_t newCap = vector->capacity * 2;
    StatData *newData = realloc(vector->data, newCap * sizeof(StatData));
    if (!newData)
      return -1;
    vector->data = newData;
    vector->capacity = newCap;
  }
  vector->data[vector->size] = *data;
  vector->size++;
  return 0;
}

int DataVectorSetData(DataVector *vector, StatData *data, size_t size) {
  if (!vector || !data || !(vector->data)) {
    return -1;
  }
  SafeRelease(vector->data);
  vector->size = size;
  vector->capacity = size;
  vector->data = data;
  return 0;
}

int DataVectorPop(DataVector *vector, StatData *data) {
  if (vector->size == 0)
    return -1;
  if (vector->size * 2 == vector->capacity) {
    void *res = realloc(vector->data, vector->size * sizeof(StatData));
    if (!res) {
      return -1;
    }
    vector = res;
  }
  vector->size -= 1;
  *data = (vector->data[vector->size]);
  return 0;
}

void DataVectorDestroy(DataVector *vector) {
  if (!vector)
    return;
  SafeRelease(vector->data);
  SafeRelease(vector);
}

int StoreDump(const char *name, const DataVector *arr) {
  if (!name || !arr) {
    char msg[256];
    snprintf(msg, sizeof(msg), "%s has invalid arguments. name: %p, arr: %p",
             __func__, (void *)name, (void *)arr);
    errno = EINVAL;
    perror(msg);
    return -1;
  }

  if (arr->size > 0 && arr->data == NULL) {
    char msg[256];
    snprintf(msg, sizeof(msg), "%s Dump is null. arr->data: %p", __func__,
             (void *)arr->data);
    errno = EINVAL;
    perror(msg);
    return -1;
  }

  FILE *fp = fopen(name, "wb");
  if (!fp) {
    perror(__func__);
    return -1;
  }

  size_t written = fwrite(arr->data, sizeof(StatData), arr->size, fp);
  if (written != arr->size) {
    perror(__func__);
    fclose(fp);
    fp = NULL;
    return -1;
  }
  fclose(fp);
  fp = NULL;
  return 0;
}

DataVector *LoadDump(const char *name) {
  if (!name) {
    char msg[256];
    snprintf(msg, sizeof(msg), "%s has invalid arguments. name: %p", __func__,
             (void *)name);
    errno = EINVAL;
    perror(msg);
    return NULL;
  }

  FILE *fp = fopen(name, "rb");
  if (!fp) {
    perror(__func__);
    return NULL;
  }

  DataVector *vector = DataVectorCreate(0);
  if (!vector) {
    perror(__func__);
    return NULL;
  }

  if (fseek(fp, 0, SEEK_END) != 0) {
    perror(__func__);
    fclose(fp);
    return NULL;
  }

  long fileSize = ftell(fp);
  if (fileSize < 0) {
    perror(__func__);
    fclose(fp);
    return NULL;
  }
  if (fileSize % sizeof(StatData) != 0) {
    errno = EBADMSG;
    perror(__func__);
    return NULL;
  }
  rewind(fp);
  StatData *data = malloc(fileSize);
  if (!data) {
    perror(__func__);
    return NULL;
  }

  size_t read_count =
      fread(data, sizeof(StatData), fileSize / sizeof(StatData), fp);
  if (read_count != fileSize / sizeof(StatData)) {
    if (ferror(fp)) {
      perror("fread");
    } else {
      fprintf(stderr,
              "fread: unexpected end of file (read %zu, expected %zu)\n",
              read_count, fileSize / sizeof(StatData));
    }
    SafeRelease(data);
    fclose(fp);
    return NULL;
  }

  fclose(fp);

  DataVectorSetData(vector, data, fileSize / sizeof(StatData));
  return vector;
}

static bool CopyUnique(DataVector *dst, const DataVector *src, RbTree **seen) {
  if (!dst || !src || !seen)
    return false;

  for (size_t i = 0; i < src->size; i++) {
    RbTree *node = RbSearch(*seen, src->data[i].id, LongCompareOf);
    if (RbIsEmpty(node)) {
      // Новый id – добавляем копию
      StatData *copy = malloc(sizeof(StatData));
      if (!copy) {
        perror("malloc copy");
        RbDestroy(*seen);
        return false;
      }
      memcpy(copy, &(src->data[i]), sizeof(StatData));
      if (DataVectorPush(dst, copy) != 0) {
        perror("DataVectorPush");
        SafeRelease(copy);
        RbDestroy(*seen);
        return false;
      }
      if (!RbInsert(seen, src->data[i].id, LongCompareOf)) {
        perror("RbInsert");
        RbDestroy(*seen);
        return false;
      }
    } else {
      // id уже существует – обновляем существующий элемент в dst
      int found_idx = -1;
      for (size_t j = 0; j < dst->size; j++) {
        if (dst->data[j].id == src->data[i].id) {
          found_idx = (int)j;
          break;
        }
      }
      if (found_idx == -1) {
        fprintf(stderr, "Internal error: id in tree but not in dst\n");
        return false;
      }
      // Суммируем count и cost
      dst->data[found_idx].count += src->data[i].count;
      dst->data[found_idx].cost += src->data[i].cost;
      // primary = логическое И
      dst->data[found_idx].primary =
          (dst->data[found_idx].primary && src->data[i].primary) ? 1 : 0;
      // mode = максимум
      if (src->data[i].mode > dst->data[found_idx].mode) {
        dst->data[found_idx].mode = src->data[i].mode;
      }
    }
  }
  return true;
}

DataVector *JoinDump(const DataVector *v1, const DataVector *v2) {
  if (!v1 || !v2) {
    char msg[256];
    snprintf(msg, sizeof(msg), "%s has invalid arguments. dst: %p, src: %p",
             __func__, (void *)v1, (void *)v2);
    errno = EINVAL;
    perror(msg);
    return NULL;
  }

  DataVector *res = DataVectorCreate(0);
  if (!res) {
    perror("DataVectorCreate with v1 failed");
    return NULL;
  }

  RbTree *seen = NULL;

  if (!CopyUnique(res, v1, &seen)) {
    perror("CopyUnique with v1 failed");
    DataVectorDestroy(res);
    return NULL;
  }

  printf("tree key %ld\n", RbKey(seen));
  printf("v1 id %ld\n", DataVectorGet(v1, 0).id);
  printf("res id %ld\n", DataVectorGet(res, 0).id);

  if (!CopyUnique(res, v2, &seen)) {
    perror("CopyUnique with v2 failed");
    DataVectorDestroy(res);
    return NULL;
  }
  RbDestroy(seen);
  return res;
}

int SortDump(DataVector *base, int (*compare)(const void *, const void *)) {
  if (!base) {
    char msg[256];
    snprintf(msg, sizeof(msg), "%s base is null.", __func__);
    errno = EINVAL;
    perror(msg);
    return -1;
  }

  if (!(base->data)) {
    char msg[256];
    snprintf(msg, sizeof(msg), "%s base->data is null.", __func__);
    errno = EINVAL;
    perror(msg);
    return -1;
  }

  if (!compare)
    compare = CostCompareOf;

  qsort(base->data, base->size, sizeof(StatData), compare);
  return 0;
}