#include "test_rbtree.h"
#include "rbtree.h"
#include "test.h"
#include <assert.h>
#include <common.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/* Вспомогательные функции (остаются static, не экспортируются) */

static int cmpLong(long a, long b) { return (a > b) - (a < b); }

static int checkRbProperties(RbTree *node) {
  if (RbIsEmpty(node))
    return 1; // чёрная высота нулевого узла = 1 (считаем, что nil – чёрный)

  if (RbColor(node) == RED) {
    if (!RbIsEmpty(RbLeft(node)) && RbColor(RbLeft(node)) == RED) {
      fprintf(stderr,
              "Нарушение: красный узел имеет красного левого потомка\n");
      return -1;
    }
    if (!RbIsEmpty(RbRight(node)) && RbColor(RbRight(node)) == RED) {
      fprintf(stderr,
              "Нарушение: красный узел имеет красного правого потомка\n");
      return -1;
    }
  }

  int left_h = checkRbProperties(RbLeft(node));
  int right_h = checkRbProperties(RbRight(node));
  if (left_h < 0 || right_h < 0)
    return -1;

  if (left_h != right_h) {
    fprintf(stderr,
            "Нарушение: чёрная высота не совпадает (left=%d, right=%d)\n",
            left_h, right_h);
    return -1;
  }

  return left_h + (RbColor(node) == BLACK ? 1 : 0);
}

static bool validateRbtree(RbTree *root) {
  if (RbIsEmpty(root))
    return true;

  if (RbColor(root) != BLACK) {
    fprintf(stderr, "Корень не чёрный\n");
    return false;
  }

  return checkRbProperties(root) >= 0;
}

static size_t countNodes(RbTree *node) {
  if (RbIsEmpty(node))
    return 0;
  return 1 + countNodes(RbLeft(node)) + countNodes(RbRight(node));
}

/* ============ Экспортируемые тестовые функции ============ */

bool TestCreateDestroy(void) {
  printf("TestCreateDestroy: ");
  bool res = true;
  clock_t start = clock();

  for (int i = 0; i < ITERATIONS; ++i) {
    RbTree *node = NewRbNode(42);
    if (node == NULL) {
      fprintf(stderr, "FAIL (NewRbNode вернул NULL)\n");
      res = false;
      goto finish;
    }
    if (RbKey(node) != 42) {
      fprintf(stderr, "FAIL (неверный ключ)\n");
      SafeRelease(node);
      res = false;
      goto finish;
    }
    if (RbColor(node) != BLACK) {
      fprintf(stderr,
              "FAIL (цвет нового узла не чёрный, хотя должен быть BLACK)\n");
      SafeRelease(node);
      res = false;
      goto finish;
    }

    RbDestroy(node);
    if (!RbIsEmpty(NULL)) {
      fprintf(stderr, "FAIL (RbIsEmpty(NULL) должна вернуть true)\n");
      res = false;
      goto finish;
    }
  }

finish:
  clock_t end = clock();
  double avg = ((double)(end - start)) / (CLOCKS_PER_SEC * ITERATIONS);
  PrintTestResult(__func__, res, avg);
  return res;
}

bool TestInsertSearch(void) {
  printf("TestInsertSearch: ");
  bool res = true;
  clock_t start = clock();

  for (int iter = 0; iter < ITERATIONS; ++iter) {
    RbTree *root = NULL;
    long keys[] = {5, 3, 7, 1, 9, 4, 6, 8, 2, 0};
    size_t n = sizeof(keys) / sizeof(keys[0]);

    for (size_t i = 0; i < n; ++i) {
      if (!RbInsert(&root, keys[i], cmpLong)) {
        fprintf(stderr, "FAIL (вставка %ld не удалась)\n", keys[i]);
        RbDestroy(root);
        res = false;
        goto finish;
      }
      if (!validateRbtree(root)) {
        fprintf(stderr, "FAIL (нарушение свойств после вставки %ld)\n",
                keys[i]);
        RbDestroy(root);
        res = false;
        goto finish;
      }
    }

    for (size_t i = 0; i < n; ++i) {
      RbTree *found = RbSearch(root, keys[i], cmpLong);
      if (RbIsEmpty(found)) {
        fprintf(stderr, "FAIL (ключ %ld не найден)\n", keys[i]);
        RbDestroy(root);
        res = false;
        goto finish;
      }
      if (RbKey(found) != keys[i]) {
        fprintf(stderr, "FAIL (найденный ключ %ld не соответствует %ld)\n",
                RbKey(found), keys[i]);
        RbDestroy(root);
        res = false;
        goto finish;
      }
    }

    RbTree *not_found = RbSearch(root, 100, cmpLong);
    if (!RbIsEmpty(not_found)) {
      fprintf(stderr, "FAIL (несуществующий ключ 100 найден)\n");
      RbDestroy(root);
      res = false;
      goto finish;
    }

    if (RbInsert(&root, 5, cmpLong)) {
      fprintf(stderr, "FAIL (вставка дубликата 5 должна была вернуть false)\n");
      RbDestroy(root);
      res = false;
      goto finish;
    }

    if (countNodes(root) != n) {
      fprintf(
          stderr,
          "FAIL (количество узлов не совпадает: ожидалось %zu, получено %zu)\n",
          n, countNodes(root));
      RbDestroy(root);
      res = false;
      goto finish;
    }

    RbDestroy(root);
  }

finish:
  clock_t end = clock();
  double avg = ((double)(end - start)) / (CLOCKS_PER_SEC * ITERATIONS);
  PrintTestResult(__func__, res, avg);
  return res;
}

bool TestDelete(void) {
  printf("TestDelete: ");
  bool res = true;
  clock_t start = clock();

  for (int iter = 0; iter < ITERATIONS; ++iter) {
    RbTree *root = NULL;
    long keys[] = {10, 5, 15, 3, 7, 12, 18, 1, 4, 6, 8, 11, 13, 16, 19};
    size_t n = sizeof(keys) / sizeof(keys[0]);

    for (size_t i = 0; i < n; ++i) {
      if (!RbInsert(&root, keys[i], cmpLong)) {
        fprintf(stderr, "FAIL (вставка %ld не удалась)\n", keys[i]);
        RbDestroy(root);
        res = false;
        goto finish;
      }
    }

    if (!validateRbtree(root)) {
      fprintf(stderr, "FAIL (нарушение свойств перед удалением)\n");
      RbDestroy(root);
      res = false;
      goto finish;
    }

    for (size_t i = 0; i < n; i += 2) {
      RbTree *node = RbSearch(root, keys[i], cmpLong);
      if (RbIsEmpty(node)) {
        fprintf(stderr, "FAIL (ключ %ld не найден для удаления)\n", keys[i]);
        RbDestroy(root);
        res = false;
        goto finish;
      }
      if (!RbDelete(&root, node)) {
        fprintf(stderr, "FAIL (удаление %ld не удалось)\n", keys[i]);
        RbDestroy(root);
        res = false;
        goto finish;
      }
      if (!validateRbtree(root)) {
        fprintf(stderr, "FAIL (нарушение свойств после удаления %ld)\n",
                keys[i]);
        RbDestroy(root);
        res = false;
        goto finish;
      }
      RbTree *found = RbSearch(root, keys[i], cmpLong);
      if (!RbIsEmpty(found)) {
        fprintf(stderr, "FAIL (ключ %ld найден после удаления)\n", keys[i]);
        RbDestroy(root);
        res = false;
        goto finish;
      }
    }

    for (size_t i = 1; i < n; i += 2) {
      RbTree *node = RbSearch(root, keys[i], cmpLong);
      if (RbIsEmpty(node)) {
        fprintf(stderr, "FAIL (ключ %ld не найден для удаления)\n", keys[i]);
        RbDestroy(root);
        res = false;
        goto finish;
      }
      if (!RbDelete(&root, node)) {
        fprintf(stderr, "FAIL (удаление %ld не удалось)\n", keys[i]);
        RbDestroy(root);
        res = false;
        goto finish;
      }
      if (!validateRbtree(root)) {
        fprintf(stderr, "FAIL (нарушение свойств после удаления %ld)\n",
                keys[i]);
        RbDestroy(root);
        res = false;
        goto finish;
      }
    }

    if (!RbIsEmpty(root)) {
      fprintf(stderr,
              "FAIL (дерево не пустое после удаления всех элементов)\n");
      RbDestroy(root);
      res = false;
      goto finish;
    }

    // Попытка удалить из пустого дерева
    RbTree *dummy = NewRbNode(999);
    if (RbDelete(&root, dummy)) {
      fprintf(stderr,
              "FAIL (удаление из пустого дерева должно вернуть false)\n");
      SafeRelease(dummy);
      RbDestroy(root);
      res = false;
      goto finish;
    }
    SafeRelease(dummy);

    if (RbDelete(&root, NULL)) {
      fprintf(stderr, "FAIL (удаление NULL должно вернуть false)\n");
      RbDestroy(root);
      res = false;
      goto finish;
    }

    RbDestroy(root);
  }

finish:
  clock_t end = clock();
  double avg = ((double)(end - start)) / (CLOCKS_PER_SEC * ITERATIONS);
  PrintTestResult(__func__, res, avg);
  return res;
}

bool TestMinMaxSuccPred(void) {
  printf("TestMinMaxSuccPred: ");
  bool res = true;
  clock_t start = clock();

  for (int iter = 0; iter < ITERATIONS; ++iter) {
    RbTree *root = NULL;
    long keys[] = {4, 2, 6, 1, 3, 5, 7};
    size_t n = sizeof(keys) / sizeof(keys[0]);

    for (size_t i = 0; i < n; ++i) {
      if (!RbInsert(&root, keys[i], cmpLong)) {
        fprintf(stderr, "FAIL (вставка %ld не удалась)\n", keys[i]);
        RbDestroy(root);
        res = false;
        goto finish;
      }
    }

    if (RbIsEmpty(RbMin(root)) || RbKey(RbMin(root)) != 1) {
      fprintf(stderr, "FAIL (минимальный ключ не 1)\n");
      RbDestroy(root);
      res = false;
      goto finish;
    }
    if (RbIsEmpty(RbMax(root)) || RbKey(RbMax(root)) != 7) {
      fprintf(stderr, "FAIL (максимальный ключ не 7)\n");
      RbDestroy(root);
      res = false;
      goto finish;
    }

    long sorted[] = {1, 2, 3, 4, 5, 6, 7};
    RbTree *node = RbMin(root);
    for (size_t i = 0; i < n; ++i) {
      if (RbIsEmpty(node) || RbKey(node) != sorted[i]) {
        fprintf(stderr,
                "FAIL (при обходе через successor ожидался %ld, получен %ld)\n",
                sorted[i], RbIsEmpty(node) ? -1 : RbKey(node));
        RbDestroy(root);
        res = false;
        goto finish;
      }
      node = RbSuccessor(node);
    }
    if (!RbIsEmpty(node)) {
      fprintf(
          stderr,
          "FAIL (после последнего элемента successor должен быть пустым)\n");
      RbDestroy(root);
      res = false;
      goto finish;
    }

    node = RbMax(root);
    for (size_t i = n; i > 0; --i) {
      if (RbIsEmpty(node) || RbKey(node) != sorted[i - 1]) {
        fprintf(
            stderr,
            "FAIL (при обходе через predecessor ожидался %ld, получен %ld)\n",
            sorted[i - 1], RbIsEmpty(node) ? -1 : RbKey(node));
        RbDestroy(root);
        res = false;
        goto finish;
      }
      node = RbPredecessor(node);
    }
    if (!RbIsEmpty(node)) {
      fprintf(stderr,
              "FAIL (после первого элемента predecessor должен быть пустым)\n");
      RbDestroy(root);
      res = false;
      goto finish;
    }

    size_t count = 0;
    for (node = RbStart(root); !RbIsEmpty(node); node = RbNext(node)) {
      ++count;
    }
    if (count != n) {
      fprintf(stderr, "FAIL (RbStart/RbNext обошло %zu узлов, ожидалось %zu)\n",
              count, n);
      RbDestroy(root);
      res = false;
      goto finish;
    }

    RbTree *mid = RbSearch(root, 4, cmpLong);
    if (!RbIsEmpty(mid)) {
      if (!RbHasNext(mid)) {
        fprintf(stderr,
                "FAIL (узел 4 имеет successor, но RbHasNext вернула false)\n");
        RbDestroy(root);
        res = false;
        goto finish;
      }
      if (!RbHasPrev(mid)) {
        fprintf(
            stderr,
            "FAIL (узел 4 имеет predecessor, но RbHasPrev вернула false)\n");
        RbDestroy(root);
        res = false;
        goto finish;
      }
    }

    RbDestroy(root);
  }

finish:
  clock_t end = clock();
  double avg = ((double)(end - start)) / (CLOCKS_PER_SEC * ITERATIONS);
  PrintTestResult(__func__, res, avg);
  return res;
}

bool TestLargeInsertDelete(void) {
  printf("TestLargeInsertDelete: ");
  bool res = true;
  clock_t start = clock();

  // Генерируем уникальные ключи один раз
  const size_t N = 100;
  long *keys = malloc(N * sizeof(long));
  if (!keys) {
    fprintf(stderr, "FAIL (не удалось выделить память)\n");
    return false;
  }

  srand((unsigned)time(NULL));
  for (size_t i = 0; i < N; ++i) {
    long val;
    bool unique;
    do {
      val = rand() % 10000;
      unique = true;
      for (size_t j = 0; j < i; ++j) {
        if (keys[j] == val) {
          unique = false;
          break;
        }
      }
    } while (!unique);
    keys[i] = val;
  }

  for (int iter = 0; iter < ITERATIONS; ++iter) {
    RbTree *root = NULL;

    for (size_t i = 0; i < N; ++i) {
      if (!RbInsert(&root, keys[i], cmpLong)) {
        fprintf(stderr, "FAIL (вставка %ld не удалась)\n", keys[i]);
        RbDestroy(root);
        res = false;
        goto finish;
      }
      if (!validateRbtree(root)) {
        fprintf(stderr, "FAIL (нарушение свойств после вставки %ld)\n",
                keys[i]);
        RbDestroy(root);
        res = false;
        goto finish;
      }
    }

    if (countNodes(root) != N) {
      fprintf(
          stderr,
          "FAIL (количество узлов не совпадает: ожидалось %zu, получено %zu)\n",
          N, countNodes(root));
      RbDestroy(root);
      res = false;
      goto finish;
    }

    for (size_t i = 0; i < N; ++i) {
      RbTree *found = RbSearch(root, keys[i], cmpLong);
      if (RbIsEmpty(found)) {
        fprintf(stderr, "FAIL (ключ %ld не найден)\n", keys[i]);
        RbDestroy(root);
        res = false;
        goto finish;
      }
    }

    for (size_t i = 0; i < N; ++i) {
      RbTree *node = RbSearch(root, keys[i], cmpLong);
      if (RbIsEmpty(node)) {
        fprintf(stderr, "FAIL (ключ %ld не найден для удаления)\n", keys[i]);
        RbDestroy(root);
        res = false;
        goto finish;
      }
      if (!RbDelete(&root, node)) {
        fprintf(stderr, "FAIL (удаление %ld не удалось)\n", keys[i]);
        RbDestroy(root);
        res = false;
        goto finish;
      }
      if (!validateRbtree(root)) {
        fprintf(stderr, "FAIL (нарушение свойств после удаления %ld)\n",
                keys[i]);
        RbDestroy(root);
        res = false;
        goto finish;
      }
    }

    if (!RbIsEmpty(root)) {
      fprintf(stderr, "FAIL (дерево не пустое после удаления всех)\n");
      RbDestroy(root);
      res = false;
      goto finish;
    }

    RbDestroy(root);
  }

finish:
  free(keys);
  clock_t end = clock();
  double avg = ((double)(end - start)) / (CLOCKS_PER_SEC * ITERATIONS);
  PrintTestResult(__func__, res, avg);
  return res;
}

bool TestNullHandling(void) {
  printf("TestNullHandling: ");
  bool res = true;
  clock_t start = clock();

  for (int iter = 0; iter < ITERATIONS; ++iter) {
    RbTree *root = NULL;

    if (!RbInsert(&root, 10, cmpLong)) {
      fprintf(stderr, "FAIL (вставка в пустое дерево вернула false)\n");
      res = false;
      goto finish;
    }

    RbTree *search_res = RbSearch(NULL, 10, cmpLong);
    if (!RbIsEmpty(search_res)) {
      fprintf(stderr, "FAIL (RbSearch(NULL, ...) не вернула пустой узел)\n");
      RbDestroy(root);
      res = false;
      goto finish;
    }

    RbTree *dummy = NewRbNode(42);
    if (RbDelete(NULL, dummy)) {
      fprintf(stderr, "FAIL (RbDelete(NULL, ...) должна вернуть false)\n");
      SafeRelease(dummy);
      RbDestroy(root);
      res = false;
      goto finish;
    }
    SafeRelease(dummy);

    if (RbDelete(&root, NULL)) {
      fprintf(stderr, "FAIL (RbDelete(..., NULL) должна вернуть false)\n");
      RbDestroy(root);
      res = false;
      goto finish;
    }

    if (!RbIsEmpty(RbMin(NULL))) {
      fprintf(stderr, "FAIL (RbMin(NULL) не вернула пустой узел)\n");
      RbDestroy(root);
      res = false;
      goto finish;
    }
    if (!RbIsEmpty(RbMax(NULL))) {
      fprintf(stderr, "FAIL (RbMax(NULL) не вернула пустой узел)\n");
      RbDestroy(root);
      res = false;
      goto finish;
    }
    if (!RbIsEmpty(RbSuccessor(NULL))) {
      fprintf(stderr, "FAIL (RbSuccessor(NULL) не вернула пустой узел)\n");
      RbDestroy(root);
      res = false;
      goto finish;
    }
    if (!RbIsEmpty(RbPredecessor(NULL))) {
      fprintf(stderr, "FAIL (RbPredecessor(NULL) не вернула пустой узел)\n");
      RbDestroy(root);
      res = false;
      goto finish;
    }
    if (!RbIsEmpty(RbStart(NULL))) {
      fprintf(stderr, "FAIL (RbStart(NULL) не вернула пустой узел)\n");
      RbDestroy(root);
      res = false;
      goto finish;
    }
    if (!RbIsEmpty(RbEnd(NULL))) {
      fprintf(stderr, "FAIL (RbEnd(NULL) не вернула пустой узел)\n");
      RbDestroy(root);
      res = false;
      goto finish;
    }
    if (RbHasNext(NULL)) {
      fprintf(stderr, "FAIL (RbHasNext(NULL) вернула true)\n");
      RbDestroy(root);
      res = false;
      goto finish;
    }
    if (RbHasPrev(NULL)) {
      fprintf(stderr, "FAIL (RbHasPrev(NULL) вернула true)\n");
      RbDestroy(root);
      res = false;
      goto finish;
    }
    if (!RbIsEmpty(RbNext(NULL))) {
      fprintf(stderr, "FAIL (RbNext(NULL) не вернула пустой узел)\n");
      RbDestroy(root);
      res = false;
      goto finish;
    }
    if (!RbIsEmpty(RbPrev(NULL))) {
      fprintf(stderr, "FAIL (RbPrev(NULL) не вернула пустой узел)\n");
      RbDestroy(root);
      res = false;
      goto finish;
    }

    RbDestroy(NULL);
    RbDestroy(root);
  }

finish:
  clock_t end = clock();
  double avg = ((double)(end - start)) / (CLOCKS_PER_SEC * ITERATIONS);
  PrintTestResult(__func__, res, avg);
  return res;
}