#include <test.h>

#include "rbtree.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <test_rbtree.h>
#include <time.h>

static bool (*dumperTests[])(void) = {
    testInvalidArgsAll,        testInvalidArgs1, testInvalidArgs2,
    testInputNotExists,        testOutputExists, testOutputPermissionDenied,
    testInputPermissionDenied, testSuccess,

};

static bool (*rbtreeTests[])(void) = {
    testCreateDestroy,  testInsertSearch,      testDelete,
    testMinMaxSuccPred, testLargeInsertDelete, testNullHandling};

int main() {
  for (int i = 0; i < sizeof(rbtreeTests) / sizeof(void *); i++) {
    rbtreeTests[i]();
  }
  for (int i = 0; i < sizeof(dumperTests) / sizeof(void *); i++) {
    dumperTests[i]();
  }
  return EXIT_SUCCESS;
}
