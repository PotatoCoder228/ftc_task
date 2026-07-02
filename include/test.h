#pragma once
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <test_data.h>
static const int ITERATIONS = 10;

int runProcess(const char *cmd);

bool testInvalidArgsAll();

bool testInvalidArgs1();
bool testInvalidArgs2();
bool testInputNotExists();
bool testOutputExists();

bool testOutputPermissionDenied();
bool testInputPermissionDenied();
bool testSuccess();

void printTestResult(const char *fn, bool res, double time);

static inline void StoreTestSet(const char *fn1, const char *fn2,
                                StatData *arr1, StatData *arr2, size_t num1,
                                size_t num2) {
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