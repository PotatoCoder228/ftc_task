#include "test.h"
#include "dumper.h"
#include "test_data.h"
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int RunProcess(const char *cmd) {
  if (!cmd) {
    errno = EINVAL;
    perror(__func__);
    return -1;
  }

  FILE *fp = popen(cmd, "r");
  if (fp == NULL) {
    perror(__func__);
    return -1;
  }

  // Читаем и игнорируем вывод, чтобы избежать SIGPIPE
  char buf[256];
  while (fgets(buf, sizeof(buf), fp) != NULL) {
    // printf(buf);
  }

  int status = pclose(fp);
  if (status == -1) {
    perror(__func__);
    return -1;
  }

  if (WIFEXITED(status)) {
    return WEXITSTATUS(status);
  } else if (WIFSIGNALED(status)) {
    fprintf(stderr, "Process terminated by signal %d\n", WTERMSIG(status));
    return -1;
  } else {
    fprintf(stderr, "Process stopped unexpectedly\n");
    return -1;
  }
}

void cleanup() {
  RunProcess("chmod 700 *.dump 2>/dev/null");
  RunProcess("rm *.dump 2>/dev/null");
}

void PrintTestResult(const char *fn, bool res, double time) {
  if (res) {
    printf("%s is success. Time: %fs\n", fn, time);
  } else {
    printf("%s failed. Time: %fs\n", fn, time);
  }
}

bool TestInvalidArgsAll() {
  bool res = false;
  clock_t start = clock();
  for (int i = 0; i < ITERATIONS; i++) {
    res = RunProcess("./build/ftc_dump 2>/dev/null") != 0;
    cleanup();
  }
  clock_t end = clock();
  double avg = ((double)(end - start)) / (CLOCKS_PER_SEC * ITERATIONS);

  PrintTestResult(__func__, res, avg);
  return res;
}

bool TestInvalidArgs1() {
  bool res = false;
  clock_t start = clock();
  for (int i = 0; i < ITERATIONS; i++) {
    res = RunProcess("./build/ftc_dump set1.dump 2>/dev/null") != 0;
    cleanup();
  }
  clock_t end = clock();
  double avg = ((double)(end - start)) / (CLOCKS_PER_SEC * ITERATIONS);

  PrintTestResult(__func__, res, avg);
  return res;
}

bool TestInvalidArgs2() {
  bool res = false;
  clock_t start = clock();
  for (int i = 0; i < ITERATIONS; i++) {
    res = RunProcess("./build/ftc_dump set1.dump set2.dump 2>/dev/null") != 0;
    cleanup();
  }
  clock_t end = clock();
  double avg = ((double)(end - start)) / (CLOCKS_PER_SEC * ITERATIONS);

  PrintTestResult(__func__, res, avg);
  return res;
}

bool TestInputNotExists() {
  bool res = false;
  clock_t start = clock();
  for (int i = 0; i < ITERATIONS; i++) {
    res =
        RunProcess(
            "./build/ftc_dump set1.dump set2.dump set3.dump 2>/dev/null") != 0;
    cleanup();
  }
  clock_t end = clock();
  double avg = ((double)(end - start)) / (CLOCKS_PER_SEC * ITERATIONS);

  PrintTestResult(__func__, res, avg);
  return res;
}

bool TestOutputExists() {
  bool res = false;
  clock_t start = clock();
  for (int i = 0; i < ITERATIONS; i++) {
    StoreTestSet("set1.dump", "set2.dump", case1InA, case1InB,
                 sizeof(case1InA) / sizeof(StatData),
                 sizeof(case1InB) / sizeof(StatData));
    RunProcess("touch set3.dump 2>/dev/null");
    res =
        RunProcess(
            "./build/ftc_dump set1.dump set2.dump set3.dump 2>/dev/null") == 0;
    DataVector *v = LoadDump("set3.dump");
    if (v) {
      if (DataVectorSize(v) == sizeof(case1Out) / sizeof(StatData)) {
        for (size_t i = 0; i < DataVectorSize(v); i++) {
          StatData data1 = DataVectorGet(v, i);
          StatData data2 = case1Out[i];
          if ((data1.id == data2.id) && (data1.count == data2.count) &&
              StatDataCostEquals(data1.cost, data2.cost) &&
              (data1.primary == data2.primary) && (data1.mode == data2.mode)) {
            continue;
          }
        }
      } else {
        fprintf(stderr, "FAIL (TestOutputExists): wrong vector size\n");
        res = false;
      }
    } else {
      fprintf(stderr, "FAIL (TestOutputExists): LoadDump returned NULL\n");
      res = false;
    }
    DataVectorDestroy(v);
    cleanup();
  }
  clock_t end = clock();
  double avg = ((double)(end - start)) / (CLOCKS_PER_SEC * ITERATIONS);

  PrintTestResult(__func__, res, avg);
  return res;
}

bool TestOutputPermissionDenied() {
  bool res = false;
  clock_t start = clock();
  for (int i = 0; i < ITERATIONS; i++) {
    StoreTestSet("set1.dump", "set2.dump", case1InA, case1InB,
                 sizeof(case1InA) / sizeof(StatData),
                 sizeof(case1InB) / sizeof(StatData));
    RunProcess("touch set3.dump 2>/dev/null");
    RunProcess("chmod -w set3.dump 2>/dev/null");
    res =
        RunProcess(
            "./build/ftc_dump set1.dump set2.dump set3.dump 2>/dev/null") != 0;
    cleanup();
  }
  clock_t end = clock();
  double avg = ((double)(end - start)) / (CLOCKS_PER_SEC * ITERATIONS);

  PrintTestResult(__func__, res, avg);
  return res;
}

bool TestInputPermissionDenied() {
  bool res = false;
  clock_t start = clock();
  for (int i = 0; i < ITERATIONS; i++) {
    StoreTestSet("set1.dump", "set2.dump", case1InA, case1InB,
                 sizeof(case1InA) / sizeof(StatData),
                 sizeof(case1InB) / sizeof(StatData));
    RunProcess("chmod a-r set1.dump 2>/dev/null");
    RunProcess("chmod a-r set2.dump 2>/dev/null");
    res =
        RunProcess(
            "./build/ftc_dump set1.dump set2.dump set3.dump 2>/dev/null") != 0;
    cleanup();
  }
  clock_t end = clock();
  double avg = ((double)(end - start)) / (CLOCKS_PER_SEC * ITERATIONS);

  PrintTestResult(__func__, res, avg);
  return res;
}

bool TestSuccess() {
  bool res = true;
  clock_t start = clock();

  for (int i = 0; i < ITERATIONS; ++i) {
    StoreTestSet("set1.dump", "set2.dump", case1InA, case1InB,
                 sizeof(case1InA) / sizeof(StatData),
                 sizeof(case1InB) / sizeof(StatData));

    int code = RunProcess(
        "./build/ftc_dump set1.dump set2.dump set3.dump 2>/dev/null");
    if (code != 0) {
      fprintf(stderr, "RunProcess failed with code %d\n", code);
      res = false;
      // cleanup();
      continue;
    }

    DataVector *v = LoadDump("set3.dump");
    if (!v) {
      fprintf(stderr, "LoadDump returned NULL\n");
      res = false;
      cleanup();
      continue;
    }

    size_t expected_size = sizeof(case1Out) / sizeof(StatData);
    if (DataVectorSize(v) != expected_size) {
      fprintf(stderr, "Size mismatch: got %zu, expected %zu\n",
              DataVectorSize(v), expected_size);
      DataVectorDestroy(v);
      res = false;
      cleanup();
      continue;
    }

    bool mismatch = false;
    for (size_t j = 0; j < expected_size; ++j) {
      StatData actual = DataVectorGet(v, j);
      StatData expected = case1Out[j];
      if (actual.id != expected.id || actual.count != expected.count ||
          !StatDataCostEquals(actual.cost, expected.cost) ||
          actual.primary != expected.primary || actual.mode != expected.mode) {
        fprintf(stderr, "Data mismatch at index %zu\n", j);
        mismatch = true;
        break;
      }
    }

    if (mismatch) {
      res = false;
    }

    DataVectorDestroy(v);
    // cleanup();
  }

  clock_t end = clock();
  double avg = ((double)(end - start)) / (CLOCKS_PER_SEC * ITERATIONS);
  PrintTestResult(__func__, res, avg);
  return res;
}

bool testWrongInput() {
  bool res = false;
  clock_t start = clock();
  for (int i = 0; i < ITERATIONS; i++) {
    RunProcess("echo testWrongInput1 > set1.dump");
    RunProcess("echo testWrongInput2 > set2.dump");
    res =
        RunProcess(
            "./build/ftc_dump set1.dump set2.dump set3.dump 2>/dev/null") != 0;
    cleanup();
  }
  clock_t end = clock();
  double avg = ((double)(end - start)) / (CLOCKS_PER_SEC * ITERATIONS);

  PrintTestResult(__func__, res, avg);
  return res;
}