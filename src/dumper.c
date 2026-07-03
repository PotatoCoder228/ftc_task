#include "dumper.h"
#include "common.h"
#include "datavector.h"
#include "rbtree.h"
#include "statdata.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Вспомогательная функция */
static bool CopyUnique(DataVector *dst, const DataVector *src, RbTree **seen) {
  if (!dst || !src || !seen)
    return false;

  size_t src_size = DataVectorSize(src);
  for (size_t i = 0; i < src_size; ++i) {
    StatData elem = DataVectorGet(src, i);
    long id = StatDataGetId(&elem);
    RbTree *node = RbSearch(*seen, id, LongCompare);
    if (RbIsEmpty(node)) {
      // Новый id – добавляем копию
      if (DataVectorPush(dst, &elem) != 0) {
        perror("DataVectorPush");
        RbDestroy(*seen);
        return false;
      }
      if (!RbInsert(seen, id, LongCompare)) {
        perror("RbInsert");
        RbDestroy(*seen);
        return false;
      }
    } else {
      // id уже существует – ищем в dst и обновляем
      size_t dst_size = DataVectorSize(dst);
      int found_idx = -1;
      for (size_t j = 0; j < dst_size; ++j) {
        StatData existing = DataVectorGet(dst, j);
        if (StatDataGetId(&existing) == id) {
          found_idx = (int)j;
          break;
        }
      }
      if (found_idx == -1) {
        fprintf(stderr, "Internal error: id in tree but not in dst\n");
        return false;
      }
      // Обновляем через геттеры/сеттеры
      StatData updated = DataVectorGet(dst, (size_t)found_idx);
      StatDataSetCount(&updated,
                       StatDataGetCount(&updated) + StatDataGetCount(&elem));
      StatDataSetCost(&updated,
                      StatDataGetCost(&updated) + StatDataGetCost(&elem));
      StatDataSetPrimary(
          &updated,
          (StatDataGetPrimary(&updated) && StatDataGetPrimary(&elem)) ? 1 : 0);
      if (StatDataGetMode(&elem) > StatDataGetMode(&updated)) {
        StatDataSetMode(&updated, StatDataGetMode(&elem));
      }
      DataVectorSet(dst, (size_t)found_idx, &updated);
    }
  }
  return true;
}

/* ---- Экспортируемые функции ---- */
int StoreDump(const char *name, const DataVector *arr) {
  if (!name || !arr) {
    errno = EINVAL;
    perror("StoreDump: invalid arguments");
    return -1;
  }

  FILE *fp = fopen(name, "wb");
  if (!fp) {
    perror("StoreDump: fopen");
    return -1;
  }

  size_t n = DataVectorSize(arr);
  const StatData *data = DataVectorGetConstData(arr);
  size_t written = fwrite(data, sizeof(StatData), n, fp);
  if (written != n) {
    perror("StoreDump: fwrite");
    fclose(fp);
    return -1;
  }
  fclose(fp);
  return 0;
}

DataVector *LoadDump(const char *name) {
  if (!name) {
    errno = EINVAL;
    perror(__func__);
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
    fclose(fp);
    return NULL;
  }

  if (fseek(fp, 0, SEEK_END) != 0) {
    perror(__func__);
    fclose(fp);
    DataVectorDestroy(vector);
    return NULL;
  }

  long fileSize = ftell(fp);
  if (fileSize < 0) {
    perror(__func__);
    fclose(fp);
    DataVectorDestroy(vector);
    return NULL;
  }
  if (fileSize % sizeof(StatData) != 0) {
    errno = EBADMSG;
    perror(__func__);
    fclose(fp);
    DataVectorDestroy(vector);
    return NULL;
  }

  rewind(fp);
  size_t count = fileSize / sizeof(StatData);
  StatData *data = malloc(fileSize);
  if (!data) {
    perror(__func__);
    fclose(fp);
    DataVectorDestroy(vector);
    return NULL;
  }

  size_t read_count = fread(data, sizeof(StatData), count, fp);
  if (read_count != count) {
    if (ferror(fp))
      perror("fread");
    else
      fprintf(stderr, "Unexpected EOF\n");
    free(data);
    fclose(fp);
    DataVectorDestroy(vector);
    return NULL;
  }

  fclose(fp);
  DataVectorSetData(vector, data, count);
  return vector;
}

DataVector *JoinDump(const DataVector *v1, const DataVector *v2) {
  if (!v1 || !v2) {
    errno = EINVAL;
    perror("JoinDump: invalid arguments");
    return NULL;
  }

  DataVector *result = DataVectorCreate(0);
  if (!result) {
    perror("JoinDump: DataVectorCreate");
    return NULL;
  }

  RbTree *seen = NULL;
  if (!CopyUnique(result, v1, &seen)) {
    DataVectorDestroy(result);
    RbDestroy(seen);
    return NULL;
  }
  if (!CopyUnique(result, v2, &seen)) {
    DataVectorDestroy(result);
    RbDestroy(seen);
    return NULL;
  }
  RbDestroy(seen);
  return result;
}

int SortDump(DataVector *base, int (*compare)(const void *, const void *)) {
  if (!base) {
    errno = EINVAL;
    perror("SortDump: base is NULL");
    return -1;
  }
  if (!compare)
    compare = StatDataCompareByCost;
  StatData *data = DataVectorGetMutableData(base);
  size_t n = DataVectorSize(base);
  qsort(data, n, sizeof(StatData), compare);
  return 0;
}