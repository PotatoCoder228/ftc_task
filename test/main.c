#include <test.h>

#include "rbtree.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Компаратор для long
static int cmp_long(long a, long b) {
    return (a > b) - (a < b);
}

// Вспомогательная функция для проверки свойств красно-чёрного дерева
// Возвращает чёрную высоту (количество чёрных узлов на пути) или -1 при нарушении
static int check_rb_properties(rb_tree_s *node) {
    if (rb_is_empty(node))
        return 1; // чёрная высота нулевого узла = 1 (считаем, что nil – чёрный)

    // 1) Красный узел не может иметь красных детей
    if (rb_color(node) == RED) {
        if (!rb_is_empty(rb_left(node)) && rb_color(rb_left(node)) == RED) {
            fprintf(stderr, "Нарушение: красный узел имеет красного левого потомка\n");
            return -1;
        }
        if (!rb_is_empty(rb_right(node)) && rb_color(rb_right(node)) == RED) {
            fprintf(stderr, "Нарушение: красный узел имеет красного правого потомка\n");
            return -1;
        }
    }

    int left_h = check_rb_properties(rb_left(node));
    int right_h = check_rb_properties(rb_right(node));
    if (left_h < 0 || right_h < 0)
        return -1;

    if (left_h != right_h) {
        fprintf(stderr, "Нарушение: чёрная высота не совпадает (left=%d, right=%d)\n",
                left_h, right_h);
        return -1;
    }

    return left_h + (rb_color(node) == BLACK ? 1 : 0);
}

// Проверка свойств для всего дерева
static bool validate_rbtree(rb_tree_s *root) {
    if (rb_is_empty(root))
        return true;

    if (rb_color(root) != BLACK) {
        fprintf(stderr, "Корень не чёрный\n");
        return false;
    }

    return check_rb_properties(root) >= 0;
}

// Подсчёт узлов
static size_t count_nodes(rb_tree_s *node) {
    if (rb_is_empty(node))
        return 0;
    return 1 + count_nodes(rb_left(node)) + count_nodes(rb_right(node));
}

// ============= Тесты =============

static bool test_create_destroy() {
    printf("test_create_destroy: ");
    rb_tree_s *node = new_rb_node(42);
    if (node == NULL) {
        printf("FAIL (new_rb_node вернул NULL)\n");
        return false;
    }
    if (rb_key(node) != 42) {
        printf("FAIL (неверный ключ)\n");
        free(node);
        return false;
    }
    if (rb_color(node) != BLACK) {
        printf("FAIL (цвет нового узла не чёрный, хотя должен быть BLACK)\n");
        free(node);
        return false;
    }

    rb_destroy(node);
    // После уничтожения указатель node недействителен, его не используем.
    // Проверяем только работу с NULL
    if (!rb_is_empty(NULL)) {
        printf("FAIL (rb_is_empty(NULL) должна вернуть true)\n");
        return false;
    }

    printf("PASS\n");
    return true;
}

static bool test_insert_search() {
    printf("test_insert_search: ");
    rb_tree_s *root = NULL;

    long keys[] = {5, 3, 7, 1, 9, 4, 6, 8, 2, 0};
    size_t n = sizeof(keys) / sizeof(keys[0]);

    for (size_t i = 0; i < n; ++i) {
        if (!rb_insert(&root, keys[i], cmp_long)) {
            printf("FAIL (вставка %ld не удалась)\n", keys[i]);
            rb_destroy(root);
            return false;
        }
        if (!validate_rbtree(root)) {
            printf("FAIL (нарушение свойств после вставки %ld)\n", keys[i]);
            rb_destroy(root);
            return false;
        }
    }

    // Поиск всех ключей
    for (size_t i = 0; i < n; ++i) {
        rb_tree_s *found = rb_search(root, keys[i], cmp_long);
        if (rb_is_empty(found)) {
            printf("FAIL (ключ %ld не найден)\n", keys[i]);
            rb_destroy(root);
            return false;
        }
        if (rb_key(found) != keys[i]) {
            printf("FAIL (найденный ключ %ld не соответствует %ld)\n",
                   rb_key(found), keys[i]);
            rb_destroy(root);
            return false;
        }
    }

    // Поиск несуществующего
    rb_tree_s *not_found = rb_search(root, 100, cmp_long);
    if (!rb_is_empty(not_found)) {
        printf("FAIL (несуществующий ключ 100 найден)\n");
        rb_destroy(root);
        return false;
    }

    // Проверка вставки дубликата (по спецификации должна вернуть false)
    // Если реализация позволяет дубликаты, это баг, и тест упадёт.
    if (rb_insert(&root, 5, cmp_long)) {
        printf("FAIL (вставка дубликата 5 должна была вернуть false)\n");
        rb_destroy(root);
        return false;
    }

    if (count_nodes(root) != n) {
        printf("FAIL (количество узлов не совпадает: ожидалось %zu, получено %zu)\n",
               n, count_nodes(root));
        rb_destroy(root);
        return false;
    }

    rb_destroy(root);
    printf("PASS\n");
    return true;
}

static bool test_delete() {
    printf("test_delete: ");
    rb_tree_s *root = NULL;

    long keys[] = {10, 5, 15, 3, 7, 12, 18, 1, 4, 6, 8, 11, 13, 16, 19};
    size_t n = sizeof(keys) / sizeof(keys[0]);

    for (size_t i = 0; i < n; ++i) {
        if (!rb_insert(&root, keys[i], cmp_long)) {
            printf("FAIL (вставка %ld не удалась)\n", keys[i]);
            rb_destroy(root);
            return false;
        }
    }

    if (!validate_rbtree(root)) {
        printf("FAIL (нарушение свойств перед удалением)\n");
        rb_destroy(root);
        return false;
    }

    // Удаляем каждый второй элемент
    for (size_t i = 0; i < n; i += 2) {
        rb_tree_s *node = rb_search(root, keys[i], cmp_long);
        if (rb_is_empty(node)) {
            printf("FAIL (ключ %ld не найден для удаления)\n", keys[i]);
            rb_destroy(root);
            return false;
        }
        if (!rb_delete(&root, node)) {
            printf("FAIL (удаление %ld не удалось)\n", keys[i]);
            rb_destroy(root);
            return false;
        }
        if (!validate_rbtree(root)) {
            printf("FAIL (нарушение свойств после удаления %ld)\n", keys[i]);
            rb_destroy(root);
            return false;
        }
        // Проверяем, что удалённый ключ больше не находится
        rb_tree_s *found = rb_search(root, keys[i], cmp_long);
        if (!rb_is_empty(found)) {
            printf("FAIL (ключ %ld найден после удаления)\n", keys[i]);
            rb_destroy(root);
            return false;
        }
    }

    // Удаляем оставшиеся
    for (size_t i = 1; i < n; i += 2) {
        rb_tree_s *node = rb_search(root, keys[i], cmp_long);
        if (rb_is_empty(node)) {
            printf("FAIL (ключ %ld не найден для удаления)\n", keys[i]);
            rb_destroy(root);
            return false;
        }
        if (!rb_delete(&root, node)) {
            printf("FAIL (удаление %ld не удалось)\n", keys[i]);
            rb_destroy(root);
            return false;
        }
        if (!validate_rbtree(root)) {
            printf("FAIL (нарушение свойств после удаления %ld)\n", keys[i]);
            rb_destroy(root);
            return false;
        }
    }

    if (!rb_is_empty(root)) {
        printf("FAIL (дерево не пустое после удаления всех элементов)\n");
        rb_destroy(root);
        return false;
    }

    // Попытка удалить из пустого дерева
    rb_tree_s *dummy = new_rb_node(999);
    if (rb_delete(&root, dummy)) {
        printf("FAIL (удаление из пустого дерева должно вернуть false)\n");
        free(dummy);
        rb_destroy(root);
        return false;
    }
    free(dummy);

    // Попытка удалить NULL
    if (rb_delete(&root, NULL)) {
        printf("FAIL (удаление NULL должно вернуть false)\n");
        rb_destroy(root);
        return false;
    }

    rb_destroy(root);
    printf("PASS\n");
    return true;
}

static bool test_min_max_succ_pred() {
    printf("test_min_max_succ_pred: ");
    rb_tree_s *root = NULL;
    long keys[] = {4, 2, 6, 1, 3, 5, 7};
    size_t n = sizeof(keys) / sizeof(keys[0]);

    for (size_t i = 0; i < n; ++i) {
        if (!rb_insert(&root, keys[i], cmp_long)) {
            printf("FAIL (вставка %ld не удалась)\n", keys[i]);
            rb_destroy(root);
            return false;
        }
    }

    if (rb_is_empty(rb_min(root)) || rb_key(rb_min(root)) != 1) {
        printf("FAIL (минимальный ключ не 1)\n");
        rb_destroy(root);
        return false;
    }
    if (rb_is_empty(rb_max(root)) || rb_key(rb_max(root)) != 7) {
        printf("FAIL (максимальный ключ не 7)\n");
        rb_destroy(root);
        return false;
    }

    long sorted[] = {1, 2, 3, 4, 5, 6, 7};
    rb_tree_s *node = rb_min(root);
    for (size_t i = 0; i < n; ++i) {
        if (rb_is_empty(node) || rb_key(node) != sorted[i]) {
            printf("FAIL (при обходе через successor ожидался %ld, получен %ld)\n",
                   sorted[i], rb_is_empty(node) ? -1 : rb_key(node));
            rb_destroy(root);
            return false;
        }
        node = rb_successor(node);
    }
    if (!rb_is_empty(node)) {
        printf("FAIL (после последнего элемента successor должен быть пустым)\n");
        rb_destroy(root);
        return false;
    }

    // predecessor в обратном порядке
    node = rb_max(root);
    for (size_t i = n; i > 0; --i) {
        if (rb_is_empty(node) || rb_key(node) != sorted[i-1]) {
            printf("FAIL (при обходе через predecessor ожидался %ld, получен %ld)\n",
                   sorted[i-1], rb_is_empty(node) ? -1 : rb_key(node));
            rb_destroy(root);
            return false;
        }
        node = rb_predecessor(node);
    }
    if (!rb_is_empty(node)) {
        printf("FAIL (после первого элемента predecessor должен быть пустым)\n");
        rb_destroy(root);
        return false;
    }

    // Проверка rb_start/rb_next
    size_t count = 0;
    for (node = rb_start(root); !rb_is_empty(node); node = rb_next(node)) {
        ++count;
    }
    if (count != n) {
        printf("FAIL (rb_start/rb_next обошло %zu узлов, ожидалось %zu)\n", count, n);
        rb_destroy(root);
        return false;
    }

    // Проверка rb_has_next / rb_has_prev
    rb_tree_s *mid = rb_search(root, 4, cmp_long);
    if (!rb_is_empty(mid)) {
        if (!rb_has_next(mid)) {
            printf("FAIL (узел 4 имеет successor, но rb_has_next вернула false)\n");
            rb_destroy(root);
            return false;
        }
        if (!rb_has_prev(mid)) {
            printf("FAIL (узел 4 имеет predecessor, но rb_has_prev вернула false)\n");
            rb_destroy(root);
            return false;
        }
    }

    rb_destroy(root);
    printf("PASS\n");
    return true;
}

static bool test_large_insert_delete() {
    printf("test_large_insert_delete: ");
    rb_tree_s *root = NULL;
    const size_t N = 100;
    long *keys = malloc(N * sizeof(long));
    if (!keys) {
        printf("FAIL (не удалось выделить память)\n");
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

    for (size_t i = 0; i < N; ++i) {
        if (!rb_insert(&root, keys[i], cmp_long)) {
            printf("FAIL (вставка %ld не удалась)\n", keys[i]);
            free(keys);
            rb_destroy(root);
            return false;
        }
        if (!validate_rbtree(root)) {
            printf("FAIL (нарушение свойств после вставки %ld)\n", keys[i]);
            free(keys);
            rb_destroy(root);
            return false;
        }
    }

    if (count_nodes(root) != N) {
        printf("FAIL (количество узлов не совпадает: ожидалось %zu, получено %zu)\n",
               N, count_nodes(root));
        free(keys);
        rb_destroy(root);
        return false;
    }

    for (size_t i = 0; i < N; ++i) {
        rb_tree_s *found = rb_search(root, keys[i], cmp_long);
        if (rb_is_empty(found)) {
            printf("FAIL (ключ %ld не найден)\n", keys[i]);
            free(keys);
            rb_destroy(root);
            return false;
        }
    }

    for (size_t i = 0; i < N; ++i) {
        rb_tree_s *node = rb_search(root, keys[i], cmp_long);
        if (rb_is_empty(node)) {
            printf("FAIL (ключ %ld не найден для удаления)\n", keys[i]);
            free(keys);
            rb_destroy(root);
            return false;
        }
        if (!rb_delete(&root, node)) {
            printf("FAIL (удаление %ld не удалось)\n", keys[i]);
            free(keys);
            rb_destroy(root);
            return false;
        }
        if (!validate_rbtree(root)) {
            printf("FAIL (нарушение свойств после удаления %ld)\n", keys[i]);
            free(keys);
            rb_destroy(root);
            return false;
        }
    }

    if (!rb_is_empty(root)) {
        printf("FAIL (дерево не пустое после удаления всех)\n");
        free(keys);
        rb_destroy(root);
        return false;
    }

    free(keys);
    rb_destroy(root);
    printf("PASS\n");
    return true;
}

static bool test_null_handling() {
    printf("test_null_handling: ");
    rb_tree_s *root = NULL;

    if (!rb_insert(&root, 10, cmp_long)) {
        printf("FAIL (вставка в пустое дерево вернула false)\n");
        return false;
    }

    // rb_search с нулевым корнем
    rb_tree_s *res = rb_search(NULL, 10, cmp_long);
    if (!rb_is_empty(res)) {
        printf("FAIL (rb_search(NULL, ...) не вернула пустой узел)\n");
        rb_destroy(root);
        return false;
    }

    // rb_delete с нулевым корнем
    rb_tree_s *dummy = new_rb_node(42);
    if (rb_delete(NULL, dummy)) {
        printf("FAIL (rb_delete(NULL, ...) должна вернуть false)\n");
        free(dummy);
        rb_destroy(root);
        return false;
    }
    free(dummy);

    if (rb_delete(&root, NULL)) {
        printf("FAIL (rb_delete(..., NULL) должна вернуть false)\n");
        rb_destroy(root);
        return false;
    }

    if (!rb_is_empty(rb_min(NULL))) {
        printf("FAIL (rb_min(NULL) не вернула пустой узел)\n");
        rb_destroy(root);
        return false;
    }

    if (!rb_is_empty(rb_max(NULL))) {
        printf("FAIL (rb_max(NULL) не вернула пустой узел)\n");
        rb_destroy(root);
        return false;
    }

    if (!rb_is_empty(rb_successor(NULL))) {
        printf("FAIL (rb_successor(NULL) не вернула пустой узел)\n");
        rb_destroy(root);
        return false;
    }

    if (!rb_is_empty(rb_predecessor(NULL))) {
        printf("FAIL (rb_predecessor(NULL) не вернула пустой узел)\n");
        rb_destroy(root);
        return false;
    }

    if (!rb_is_empty(rb_start(NULL))) {
        printf("FAIL (rb_start(NULL) не вернула пустой узел)\n");
        rb_destroy(root);
        return false;
    }

    if (!rb_is_empty(rb_end(NULL))) {
        printf("FAIL (rb_end(NULL) не вернула пустой узел)\n");
        rb_destroy(root);
        return false;
    }

    if (rb_has_next(NULL)) {
        printf("FAIL (rb_has_next(NULL) вернула true)\n");
        rb_destroy(root);
        return false;
    }

    if (rb_has_prev(NULL)) {
        printf("FAIL (rb_has_prev(NULL) вернула true)\n");
        rb_destroy(root);
        return false;
    }

    if (!rb_is_empty(rb_next(NULL))) {
        printf("FAIL (rb_next(NULL) не вернула пустой узел)\n");
        rb_destroy(root);
        return false;
    }

    if (!rb_is_empty(rb_prev(NULL))) {
        printf("FAIL (rb_prev(NULL) не вернула пустой узел)\n");
        rb_destroy(root);
        return false;
    }

    rb_destroy(NULL); // не должна падать

    rb_destroy(root);
    printf("PASS\n");
    return true;
}

int main() {
  testInvalidArgsAll();
  testInvalidArgs1();
  testInvalidArgs2();
  testInputNotExists();
  testOutputExists();
  testOutputPermissionDenied();
  testInputPermissionDenied();
  testSuccess();

  bool all_passed = true;

  all_passed = test_create_destroy() && all_passed;
  all_passed = test_insert_search() && all_passed;
  all_passed = test_delete() && all_passed;
  all_passed = test_min_max_succ_pred() && all_passed;
  all_passed = test_large_insert_delete() && all_passed;
  all_passed = test_null_handling() && all_passed;

  if (all_passed) {
    printf("\nВсе тесты rbtree пройдены успешно.\n");
    return EXIT_SUCCESS;
  } else {
    printf("\nНекоторые rbtree тесты не прошли.\n");
    return EXIT_FAILURE;
  }
}
