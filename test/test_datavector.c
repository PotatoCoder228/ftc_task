#include "test_datavector.h"
#include "common.h"
#include "datavector.h"
#include "test.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* ============ Helper functions ============ */

/**
 * @brief Compares two StatData structures for equality.
 * @param a First StatData.
 * @param b Second StatData.
 * @return true if all fields are equal, false otherwise.
 */
static bool statDataEqual(const StatData *a, const StatData *b) {
  return (StatDataGetId(a) == StatDataGetId(b) &&
          StatDataGetCount(a) == StatDataGetCount(b) &&
          StatDataGetCost(a) == StatDataGetCost(b) &&
          StatDataGetPrimary(a) == StatDataGetPrimary(b) &&
          StatDataGetMode(a) == StatDataGetMode(b));
}

/**
 * @brief Creates a StatData structure with given values.
 */
static StatData makeStatData(long id, int count, float cost,
                             unsigned int primary, unsigned int mode) {
  StatData sd;
  StatDataInit(&sd, id, count, cost, primary, mode);
  return sd;
}

/* ============ Тесты ============ */

bool TestDataVectorCreateDestroy(void) {
  printf("TestDataVectorCreateDestroy: ");
  bool res = true;
  clock_t start = clock();

  for (int i = 0; i < ITERATIONS; ++i) {
    // Создание с нулевым размером – должна использоваться дефолтная ёмкость
    DataVector *v = DataVectorCreate(0);
    if (v == NULL) {
      fprintf(stderr, "FAIL (DataVectorCreate вернул NULL)\n");
      res = false;
      goto finish;
    }
    if (DataVectorSize(v) != 0) {
      fprintf(stderr, "FAIL (размер нового вектора не 0)\n");
      DataVectorDestroy(v);
      res = false;
      goto finish;
    }

    DataVectorDestroy(v);

    // Создание с положительным размером
    v = DataVectorCreate(5);
    if (v == NULL) {
      fprintf(stderr, "FAIL (DataVectorCreate(5) вернул NULL)\n");
      res = false;
      goto finish;
    }
    if (DataVectorSize(v) != 0) {
      fprintf(stderr,
              "FAIL (размер нового вектора не 0 при создании с ёмкостью)\n");
      DataVectorDestroy(v);
      res = false;
      goto finish;
    }
    DataVectorDestroy(v);

    // Проверка на NULL
    DataVectorDestroy(NULL); // не должно падать
  }

finish:
  clock_t end = clock();
  double avg = ((double)(end - start)) / (CLOCKS_PER_SEC * ITERATIONS);
  PrintTestResult(__func__, res, avg);
  return res;
}

bool TestDataVectorPushPop(void) {
  printf("TestDataVectorPushPop: ");
  bool res = true;
  clock_t start = clock();

  for (int iter = 0; iter < ITERATIONS; ++iter) {
    DataVector *v =
        DataVectorCreate(2); // маленькая ёмкость, чтобы проверить рост
    if (v == NULL) {
      fprintf(stderr, "FAIL (DataVectorCreate)\n");
      res = false;
      goto finish;
    }

    // Заполняем 10 элементами
    for (int i = 0; i < 10; ++i) {
      StatData sd = makeStatData(i, i * 2, i * 0.5f, i % 2, i % 8);
      if (DataVectorPush(v, &sd) != 0) {
        fprintf(stderr, "FAIL (Push на шаге %d)\n", i);
        DataVectorDestroy(v);
        res = false;
        goto finish;
      }
      if (DataVectorSize(v) != (size_t)(i + 1)) {
        fprintf(stderr,
                "FAIL (размер после Push %d: ожидалось %d, получено %zu)\n", i,
                i + 1, DataVectorSize(v));
        DataVectorDestroy(v);
        res = false;
        goto finish;
      }
    }

    // Проверяем значения
    for (int i = 0; i < 10; ++i) {
      StatData sd = DataVectorGet(v, i);
      StatData expected = makeStatData(i, i * 2, i * 0.5f, i % 2, i % 8);
      if (!statDataEqual(&sd, &expected)) {
        fprintf(stderr, "FAIL (несовпадение данных на индексе %d)\n", i);
        DataVectorDestroy(v);
        res = false;
        goto finish;
      }
    }

    // Pop все элементы
    for (int i = 9; i >= 0; --i) {
      StatData out;
      if (DataVectorPop(v, &out) != 0) {
        fprintf(stderr, "FAIL (Pop на шаге %d)\n", i);
        DataVectorDestroy(v);
        res = false;
        goto finish;
      }
      StatData expected = makeStatData(i, i * 2, i * 0.5f, i % 2, i % 8);
      if (!statDataEqual(&out, &expected)) {
        fprintf(stderr, "FAIL (Pop вернул неверные данные на индексе %d)\n", i);
        DataVectorDestroy(v);
        res = false;
        goto finish;
      }
      if (DataVectorSize(v) != (size_t)i) {
        fprintf(stderr,
                "FAIL (размер после Pop %d: ожидалось %d, получено %zu)\n", i,
                i, DataVectorSize(v));
        DataVectorDestroy(v);
        res = false;
        goto finish;
      }
    }

    // Попытка Pop из пустого
    StatData dummy;
    if (DataVectorPop(v, &dummy) == 0) {
      fprintf(stderr, "FAIL (Pop из пустого вектора вернул 0)\n");
      DataVectorDestroy(v);
      res = false;
      goto finish;
    }

    DataVectorDestroy(v);
  }

finish:
  clock_t end = clock();
  double avg = ((double)(end - start)) / (CLOCKS_PER_SEC * ITERATIONS);
  PrintTestResult(__func__, res, avg);
  return res;
}

bool TestDataVectorGetSet(void) {
  printf("TestDataVectorGetSet: ");
  bool res = true;
  clock_t start = clock();

  for (int iter = 0; iter < ITERATIONS; ++iter) {
    DataVector *v = DataVectorCreate(5);
    if (v == NULL) {
      fprintf(stderr, "FAIL (DataVectorCreate)\n");
      res = false;
      goto finish;
    }

    // Заполняем 5 элементами
    for (int i = 0; i < 5; ++i) {
      StatData sd = makeStatData(i, i + 1, i + 0.1f, i % 2, i % 8);
      DataVectorPush(v, &sd);
    }

    // Изменяем каждый элемент через DataVectorSet
    for (int i = 0; i < 5; ++i) {
      StatData newData =
          makeStatData(i * 10, i * 100, i * 1.5f, (i + 1) % 2, (i + 3) % 8);
      DataVectorSet(v, i, &newData);
    }

    // Проверяем
    for (int i = 0; i < 5; ++i) {
      StatData sd = DataVectorGet(v, i);
      StatData expected =
          makeStatData(i * 10, i * 100, i * 1.5f, (i + 1) % 2, (i + 3) % 8);
      if (!statDataEqual(&sd, &expected)) {
        fprintf(stderr, "FAIL (несовпадение после Set на индексе %d)\n", i);
        DataVectorDestroy(v);
        res = false;
        goto finish;
      }
    }

    // Проверка DataVectorGetConstData
    const StatData *constData = DataVectorGetConstData(v);
    if (constData == NULL) {
      fprintf(stderr, "FAIL (DataVectorGetConstData вернул NULL)\n");
      DataVectorDestroy(v);
      res = false;
      goto finish;
    }
    // Проверяем первый элемент
    StatData st = makeStatData(0, 0, 0.0f, 1, 3);
    if (!statDataEqual(&constData[0], &st)) {
      fprintf(stderr, "FAIL (DataVectorGetConstData не совпадает)\n");
      DataVectorDestroy(v);
      res = false;
      goto finish;
    }

    // Проверка DataVectorGetMutableData (для сортировки, но мы просто проверим
    // доступ)
    StatData *mutData = DataVectorGetMutableData(v);
    if (mutData == NULL) {
      fprintf(stderr, "FAIL (DataVectorGetMutableData вернул NULL)\n");
      DataVectorDestroy(v);
      res = false;
      goto finish;
    }
    // Изменяем через указатель (прямое обращение)
    mutData[0].count = 999;
    StatData sd = DataVectorGet(v, 0);
    if (StatDataGetCount(&sd) != 999) {
      fprintf(stderr, "FAIL (изменение через mutable не сработало)\n");
      DataVectorDestroy(v);
      res = false;
      goto finish;
    }

    DataVectorDestroy(v);
  }

finish:
  clock_t end = clock();
  double avg = ((double)(end - start)) / (CLOCKS_PER_SEC * ITERATIONS);
  PrintTestResult(__func__, res, avg);
  return res;
}

bool TestDataVectorLarge(void) {
  printf("TestDataVectorLarge: ");
  bool res = true;
  clock_t start = clock();

  const size_t N = 1000; // большое количество

  for (int iter = 0; iter < ITERATIONS; ++iter) {
    DataVector *v = DataVectorCreate(10);
    if (v == NULL) {
      fprintf(stderr, "FAIL (DataVectorCreate)\n");
      res = false;
      goto finish;
    }

    // Заполняем N элементами
    for (size_t i = 0; i < N; ++i) {
      StatData sd =
          makeStatData((long)i, (int)(i * 2), (float)i * 0.001f, i % 2, i % 8);
      if (DataVectorPush(v, &sd) != 0) {
        fprintf(stderr, "FAIL (Push на шаге %zu)\n", i);
        DataVectorDestroy(v);
        res = false;
        goto finish;
      }
    }

    if (DataVectorSize(v) != N) {
      fprintf(stderr, "FAIL (размер после Push: ожидалось %zu, получено %zu)\n",
              N, DataVectorSize(v));
      DataVectorDestroy(v);
      res = false;
      goto finish;
    }

    // Проверяем случайный элемент
    StatData sd = DataVectorGet(v, N / 2);
    StatData expected =
        makeStatData((long)(N / 2), (int)(N), (float)(N / 2) * 0.001f,
                     (N / 2) % 2, (N / 2) % 8);
    if (!statDataEqual(&sd, &expected)) {
      fprintf(stderr, "FAIL (несовпадение данных на индексе %zu)\n", N / 2);
      DataVectorDestroy(v);
      res = false;
      goto finish;
    }

    // Удаляем половину элементов
    for (size_t i = 0; i < N / 2; ++i) {
      StatData out;
      if (DataVectorPop(v, &out) != 0) {
        fprintf(stderr, "FAIL (Pop на шаге %zu)\n", i);
        DataVectorDestroy(v);
        res = false;
        goto finish;
      }
    }
    if (DataVectorSize(v) != N / 2) {
      fprintf(stderr, "FAIL (размер после Pop: ожидалось %zu, получено %zu)\n",
              N / 2, DataVectorSize(v));
      DataVectorDestroy(v);
      res = false;
      goto finish;
    }

    DataVectorDestroy(v);
  }

finish:
  clock_t end = clock();
  double avg = ((double)(end - start)) / (CLOCKS_PER_SEC * ITERATIONS);
  PrintTestResult(__func__, res, avg);
  return res;
}

bool TestDataVectorNullHandling(void) {
  printf("TestDataVectorNullHandling: ");
  bool res = true;
  clock_t start = clock();

  for (int iter = 0; iter < ITERATIONS; ++iter) {
    // DataVectorCreate с NULL не бывает, но проверим создание с 0
    DataVector *v = DataVectorCreate(0);
    if (v == NULL) {
      fprintf(stderr, "FAIL (DataVectorCreate(0) вернул NULL)\n");
      res = false;
      goto finish;
    }

    // DataVectorDestroy(NULL) – уже проверено, но ещё раз
    DataVectorDestroy(NULL);

    // DataVectorSize(NULL) – должна вернуть 0
    if (DataVectorSize(NULL) != 0) {
      fprintf(stderr, "FAIL (DataVectorSize(NULL) не вернула 0)\n");
      DataVectorDestroy(v);
      res = false;
      goto finish;
    }

    // DataVectorSet(NULL, 0, &sd) – должна игнорировать
    StatData sd = makeStatData(1, 2, 3.0f, 1, 2);
    DataVectorSet(NULL, 0, &sd); // не должно падать

    // DataVectorPush(NULL, &sd) – должна вернуть -1
    if (DataVectorPush(NULL, &sd) != -1) {
      fprintf(stderr, "FAIL (DataVectorPush(NULL, ...) не вернула -1)\n");
      DataVectorDestroy(v);
      res = false;
      goto finish;
    }

    // DataVectorPop(NULL, &sd) – должна вернуть -1
    if (DataVectorPop(NULL, &sd) != -1) {
      fprintf(stderr, "FAIL (DataVectorPop(NULL, ...) не вернула -1)\n");
      DataVectorDestroy(v);
      res = false;
      goto finish;
    }

    // DataVectorGetConstData(NULL) – должна вернуть NULL
    if (DataVectorGetConstData(NULL) != NULL) {
      fprintf(stderr, "FAIL (DataVectorGetConstData(NULL) не вернула NULL)\n");
      DataVectorDestroy(v);
      res = false;
      goto finish;
    }

    // DataVectorGetMutableData(NULL) – должна вернуть NULL
    if (DataVectorGetMutableData(NULL) != NULL) {
      fprintf(stderr,
              "FAIL (DataVectorGetMutableData(NULL) не вернула NULL)\n");
      DataVectorDestroy(v);
      res = false;
      goto finish;
    }

    // DataVectorSetData(NULL, NULL, 0) – должна вернуть -1
    if (DataVectorSetData(NULL, NULL, 0) != -1) {
      fprintf(stderr,
              "FAIL (DataVectorSetData(NULL, NULL, 0) не вернула -1)\n");
      DataVectorDestroy(v);
      res = false;
      goto finish;
    }

    // DataVectorSetData(v, NULL, 0) – должна вернуть -1
    if (DataVectorSetData(v, NULL, 0) != -1) {
      fprintf(stderr, "FAIL (DataVectorSetData(v, NULL, 0) не вернула -1)\n");
      DataVectorDestroy(v);
      res = false;
      goto finish;
    }

    DataVectorDestroy(v);
  }

finish:
  clock_t end = clock();
  double avg = ((double)(end - start)) / (CLOCKS_PER_SEC * ITERATIONS);
  PrintTestResult(__func__, res, avg);
  return res;
}