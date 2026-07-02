#pragma once
#include "dumper.h"
#include <stdlib.h>
#include <string.h>

static StatData case_1_in_a[5] = {{.id = 545432102L,
                                   .count = 888,
                                   .cost = 855.060227,
                                   .primary = 1,
                                   .mode = 2},
                                  {.id = 575433385L,
                                   .count = 200,
                                   .cost = 394.279086,
                                   .primary = 1,
                                   .mode = 0},
                                  {.id = 268277485L,
                                   .count = 211,
                                   .cost = 799.980498,
                                   .primary = 1,
                                   .mode = 2},
                                  {.id = 222356474L,
                                   .count = 18,
                                   .cost = 288.449440,
                                   .primary = 0,
                                   .mode = 4},
                                  {.id = 975146078L,
                                   .count = 321,
                                   .cost = 942.445848,
                                   .primary = 0,
                                   .mode = 4}};

static StatData case_1_in_b[5] = {{.id = 318945799L,
                                   .count = 143,
                                   .cost = 431.689356,
                                   .primary = 1,
                                   .mode = 1},
                                  {.id = 610278536L,
                                   .count = 885,
                                   .cost = 922.380960,
                                   .primary = 1,
                                   .mode = 0},
                                  {.id = 268277485L,
                                   .count = 10,
                                   .cost = 750.996248,
                                   .primary = 0,
                                   .mode = 6},
                                  {.id = 487691729L,
                                   .count = 829,
                                   .cost = 986.162848,
                                   .primary = 1,
                                   .mode = 2},
                                  {.id = 403620958L,
                                   .count = 998,
                                   .cost = 190.636557,
                                   .primary = 0,
                                   .mode = 0}};

static StatData case_1_out[9] = {{.id = 403620958L,
                                  .count = 998,
                                  .cost = 190.636557,
                                  .primary = 0,
                                  .mode = 0},
                                 {.id = 222356474L,
                                  .count = 18,
                                  .cost = 288.449440,
                                  .primary = 0,
                                  .mode = 4},
                                 {.id = 575433385L,
                                  .count = 200,
                                  .cost = 394.279086,
                                  .primary = 1,
                                  .mode = 0},
                                 {.id = 318945799L,
                                  .count = 143,
                                  .cost = 431.689356,
                                  .primary = 1,
                                  .mode = 1},
                                 {.id = 545432102L,
                                  .count = 888,
                                  .cost = 855.060227,
                                  .primary = 1,
                                  .mode = 2},
                                 {.id = 610278536L,
                                  .count = 885,
                                  .cost = 922.380960,
                                  .primary = 1,
                                  .mode = 0},
                                 {.id = 975146078L,
                                  .count = 321,
                                  .cost = 942.445848,
                                  .primary = 0,
                                  .mode = 4},
                                 {.id = 487691729L,
                                  .count = 829,
                                  .cost = 986.162848,
                                  .primary = 1,
                                  .mode = 2},
                                 {.id = 268277485L,
                                  .count = 221,
                                  .cost = 1550.976746,
                                  .primary = 0,
                                  .mode = 6}};

static inline void LoadTestSet(const char *fn1, const char *fn2, StatData *arr1,
                        StatData *arr2, size_t num1, size_t num2) {
  if (!fn1 || !fn2 || !arr1 || !arr2) {
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