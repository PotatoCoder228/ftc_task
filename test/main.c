#include <test.h>

#include "rbtree.h"
#include "test_datavector.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <test_rbtree.h>
#include <time.h>

static bool (*dumperTests[])(void) = {
    TestInvalidArgsAll,        TestInvalidArgs1, TestInvalidArgs2,
    TestInputNotExists,        TestOutputExists, TestOutputPermissionDenied,
    TestInputPermissionDenied, TestSuccess,

};

static bool (*rbtreeTests[])(void) = {
    TestCreateDestroy,  TestInsertSearch,      TestDelete,
    TestMinMaxSuccPred, TestLargeInsertDelete, TestNullHandling};

static bool (*dataVectorTests[])(void) = {
    TestDataVectorCreateDestroy, TestDataVectorPushPop, TestDataVectorGetSet,
    TestDataVectorLarge, TestDataVectorNullHandling};

int main() {
  int passed = 0;
  int all =
      (sizeof(rbtreeTests) + sizeof(dataVectorTests) + sizeof(dumperTests)) /
      sizeof(void *);
  for (int i = 0; i < sizeof(rbtreeTests) / sizeof(void *); i++) {
    if (rbtreeTests[i]())
      passed++;
  }
  for (int i = 0; i < sizeof(dataVectorTests) / sizeof(void *); i++) {
    if (dataVectorTests[i]())
      passed++;
  }

  for (int i = 0; i < sizeof(dumperTests) / sizeof(void *); i++) {
    if (dumperTests[i]())
      passed++;
  }

  printf("Passed %d/%d tests\n", passed, all);
  if (passed != all)
    return EXIT_FAILURE;
  return EXIT_SUCCESS;
}
