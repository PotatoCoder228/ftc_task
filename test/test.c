#include "test.h"
#include "dumper.h"
#include "test_data.h"
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static const int ITERATIONS = 10;

int runProcess(const char *cmd) {
  if (!cmd) {
    errno = EINVAL;
    perror(__func__);
  }
  FILE *fp;
  int status;

  fp = popen(cmd, "r");
  if (fp == NULL) {
    perror(__func__);
    exit(EXIT_FAILURE);
  }

  status = pclose(fp);
  if (status == -1) {
    perror(__func__);
    exit(EXIT_FAILURE);
  }

  if (WIFEXITED(status)) {
    return WEXITSTATUS(status);
  }

  return -1;
}

void cleanup() {
  runProcess("chmod +w *.dump 2>/dev/null");
  runProcess("rm *.dump 2>/dev/null");
}

void printTestResult(const char *fn, bool res, double time) {
  if (res) {
    printf("%s is success. Time: %fs\n", fn, time);
  } else {
    printf("%s failed. Time: %fs\n", fn, time);
  }
}

bool testInvalidArgsAll() {
  bool res = false;
  clock_t start = clock();
  for (int i = 0; i < ITERATIONS; i++) {
    res = runProcess("./build/ftc_dump 2>/dev/null") != 0;
    cleanup();
  }
  clock_t end = clock();
  double avg = ((double)(end - start)) / (CLOCKS_PER_SEC * ITERATIONS);

  printTestResult(__func__, res, avg);
  return res;
}

bool testInvalidArgs1() {
  bool res = false;
  clock_t start = clock();
  for (int i = 0; i < ITERATIONS; i++) {
    res = runProcess("./build/ftc_dump set1.dump 2>/dev/null") != 0;
    cleanup();
  }
  clock_t end = clock();
  double avg = ((double)(end - start)) / (CLOCKS_PER_SEC * ITERATIONS);

  printTestResult(__func__, res, avg);
  return res;
}

bool testInvalidArgs2() {
  bool res = false;
  clock_t start = clock();
  for (int i = 0; i < ITERATIONS; i++) {
    res = runProcess("./build/ftc_dump set1.dump set2.dump 2>/dev/null") != 0;
    cleanup();
  }
  clock_t end = clock();
  double avg = ((double)(end - start)) / (CLOCKS_PER_SEC * ITERATIONS);

  printTestResult(__func__, res, avg);
  return res;
}
bool testInputNotExists() {
  bool res = false;
  clock_t start = clock();
  for (int i = 0; i < ITERATIONS; i++) {
    res =
        runProcess(
            "./build/ftc_dump set1.dump set2.dump set3.dump 2>/dev/null") != 0;
    cleanup();
  }
  clock_t end = clock();
  double avg = ((double)(end - start)) / (CLOCKS_PER_SEC * ITERATIONS);

  printTestResult(__func__, res, avg);
  return res;
}

bool testOutputExists() {
  bool res = false;
  clock_t start = clock();
  for (int i = 0; i < ITERATIONS; i++) {
    LoadTestSet("set1.dump", "set2.dump", case_1_in_a, case_1_in_b,
                sizeof(case_1_in_a) / sizeof(StatData),
                sizeof(case_1_in_b) / sizeof(StatData));
    runProcess("touch set3.dump 2>/dev/null");
    res =
        runProcess(
            "./build/ftc_dump set1.dump set2.dump set3.dump 2>/dev/null") != 0;
    cleanup();
  }
  clock_t end = clock();
  double avg = ((double)(end - start)) / (CLOCKS_PER_SEC * ITERATIONS);

  printTestResult(__func__, res, avg);
  return res;
}

bool testOutputPermissionDenied() {
  bool res = false;
  clock_t start = clock();
  for (int i = 0; i < ITERATIONS; i++) {
    LoadTestSet("set1.dump", "set2.dump", case_1_in_a, case_1_in_b,
                sizeof(case_1_in_a) / sizeof(StatData),
                sizeof(case_1_in_b) / sizeof(StatData));
    runProcess("touch set3.dump 2>/dev/null");
    runProcess("chmod -w set3.dump 2>/dev/null");
    res =
        runProcess(
            "./build/ftc_dump set1.dump set2.dump set3.dump 2>/dev/null") != 0;
    cleanup();
  }
  clock_t end = clock();
  double avg = ((double)(end - start)) / (CLOCKS_PER_SEC * ITERATIONS);

  printTestResult(__func__, res, avg);
  return res;
}
bool testInputPermissionDenied() {
  bool res = false;
  clock_t start = clock();
  for (int i = 0; i < ITERATIONS; i++) {
    LoadTestSet("set1.dump", "set2.dump", case_1_in_a, case_1_in_b,
                sizeof(case_1_in_a) / sizeof(StatData),
                sizeof(case_1_in_b) / sizeof(StatData));
    runProcess("chmod -w set1.dump 2>/dev/null");
    runProcess("chmod -w set2.dump 2>/dev/null");
    res =
        runProcess(
            "./build/ftc_dump set1.dump set2.dump set3.dump 2>/dev/null") != 0;
    cleanup();
  }
  clock_t end = clock();
  double avg = ((double)(end - start)) / (CLOCKS_PER_SEC * ITERATIONS);

  printTestResult(__func__, res, avg);
  return res;
}

bool testSuccess() {
  bool res = false;
  clock_t start = clock();
  for (int i = 0; i < ITERATIONS; i++) {
    LoadTestSet("set1.dump", "set2.dump", case_1_in_a, case_1_in_b,
                sizeof(case_1_in_a) / sizeof(StatData),
                sizeof(case_1_in_b) / sizeof(StatData));
    res = runProcess(
        "./build/ftc_dump set1.dump set2.dump set3.dump 2>/dev/null");
    if (res != 0) {
      break;
    }

    DataVector *v = LoadDump("set3.dump");
    if (DataVectorSize(v) == sizeof(case_1_out) / sizeof(StatData)) {
      for (size_t i = 0; i < DataVectorSize(v); i++) {
        StatData data1 = DataVectorGet(v, i);
        StatData data2 = case_1_out[i];
        if ((data1.id == data2.id) || (data1.count == data2.count) ||
            (data1.cost == data2.cost) || (data1.primary == data2.primary) ||
            (data1.mode == data2.mode)) {
          continue;
        }
      }
    }
    DataVectorDestroy(v);
    cleanup();
  }
  clock_t end = clock();
  double avg = ((double)(end - start)) / (CLOCKS_PER_SEC * ITERATIONS);

  printTestResult(__func__, res, avg);
  return res;
}