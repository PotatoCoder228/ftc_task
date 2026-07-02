#pragma once

#include <stdbool.h>

/**
 * @brief Tests creation and destruction of a Red-Black Tree node.
 * @return true if all operations succeed, false otherwise.
 */
bool testCreateDestroy(void);

/**
 * @brief Tests insertion of elements and subsequent search operations.
 * @return true if all insertions are found and duplicates are rejected, false
 * otherwise.
 */
bool testInsertSearch(void);

/**
 * @brief Tests deletion of elements from the tree while maintaining Red-Black
 * properties.
 * @return true if all deletions succeed and the tree remains valid, false
 * otherwise.
 */
bool testDelete(void);

/**
 * @brief Tests minimum, maximum, successor, predecessor, and iterator
 * functions.
 * @return true if all order-statistic operations work correctly, false
 * otherwise.
 */
bool testMinMaxSuccPred(void);

/**
 * @brief Tests insertion and deletion of 100 random elements.
 * @return true if the tree handles large sets without violating properties,
 * false otherwise.
 */
bool testLargeInsertDelete(void);

/**
 * @brief Tests handling of NULL pointers in all public tree functions.
 * @return true if all NULL operations are handled gracefully, false otherwise.
 */
bool testNullHandling(void);