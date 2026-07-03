#pragma once

#include <stdbool.h>

/**
 * @brief Tests creation and destruction of a DataVector.
 * @return true if all operations succeed, false otherwise.
 */
bool TestDataVectorCreateDestroy(void);

/**
 * @brief Tests pushing and popping elements.
 * @return true if push/pop operations work correctly, false otherwise.
 */
bool TestDataVectorPushPop(void);

/**
 * @brief Tests getting and setting elements by index.
 * @return true if get/set operations work correctly, false otherwise.
 */
bool TestDataVectorGetSet(void);

/**
 * @brief Tests vector resizing with many elements.
 * @return true if the vector handles large number of elements without errors,
 * false otherwise.
 */
bool TestDataVectorLarge(void);

/**
 * @brief Tests handling of NULL pointers in all public functions.
 * @return true if all NULL operations are handled gracefully, false otherwise.
 */
bool TestDataVectorNullHandling(void);