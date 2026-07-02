#pragma once
#include "dumper.h"
#include <stdlib.h>
#include <string.h>

static StatData case_1_in_a[2] = {
    {.id = 90889, .count = 13, .cost = 3.567, .primary = 0, .mode = 3},
    {.id = 90089, .count = 1, .cost = 88.90, .primary = 1, .mode = 0}};
static StatData case_1_in_b[2] = {
    {.id = 90089, .count = 13, .cost = 0.011, .primary = 0, .mode = 2},
    {.id = 90189, .count = 1000, .cost = 1.00003, .primary = 1, .mode = 2}};
/* Ожидаемый результат обработки */
static StatData case_1_out[3] = {
    {.id = 90189, .count = 1000, .cost = 1.00003, .primary = 1, .mode = 2},
    {.id = 90889, .count = 13, .cost = 3.567, .primary = 0, .mode = 3},
    {.id = 90089, .count = 14, .cost = 88.911, .primary = 0, .mode = 2}};

inline void LoadTestSet(const char *fn1, const char *fn2, StatData *arr1,
                StatData *arr2, size_t num1, size_t num2) {
  if(!fn1||!fn2||!arr1||!arr2){
    return;
  }
  StatData *data1 = malloc(sizeof(StatData) * num1);
  StatData *data2 = malloc(sizeof(StatData) * num2);

  memcpy(data1, arr1, sizeof(StatData) * num1);
  memcpy(data2, arr2, sizeof(StatData) * num2);

  DataVector *v = DataVectorCreate(0);
  DataVectorSetData(v, data1, num1);
  StoreDump(fn1, v);
  DataVectorSetData(v, data2, num2);
  StoreDump(fn2, v);
  DataVectorDestroy(v);
}