#pragma once

#include "statdata.h"
#include <stddef.h>

/**
 * @brief Opaque structure representing a dynamic array of StatData elements.
 *
 * This structure hides its internal fields; use the provided functions to
 * manipulate it.
 */
typedef struct DataVector DataVector;

/* ----- Getters ----- */

/**
 * @brief Retrieves the StatData element at the given index.
 * @param v Pointer to the DataVector.
 * @param i Index (0-based).
 * @return The StatData at index i.
 */
StatData DataVectorGet(const DataVector *v, size_t i);

/**
 * @brief Returns the current number of elements in the vector.
 * @param v Pointer to the DataVector.
 * @return The number of elements.
 */
size_t DataVectorSize(const DataVector *v);

/**
 * @brief Returns a constant pointer to the internal data array (read-only).
 * @param v Pointer to the DataVector.
 * @return Pointer to the first StatData element.
 */
const StatData *DataVectorGetConstData(const DataVector *v);

/**
 * @brief Returns a mutable pointer to the internal data array (use with
 * caution).
 *
 * This function breaks encapsulation and should be used only for operations
 * like sorting where direct access is necessary. Prefer using DataVectorSet
 * for modifications.
 *
 * @param v Pointer to the DataVector.
 * @return Pointer to the first StatData element.
 */
StatData *DataVectorGetMutableData(DataVector *v);

/**
 * @brief Sets the element at the given index to a new value.
 * @param v Pointer to the DataVector.
 * @param index Index to modify.
 * @param data Pointer to the new StatData.
 */
void DataVectorSet(DataVector *v, size_t index, const StatData *data);

/* ----- Management ----- */

/**
 * @brief Creates a new empty DataVector with an initial capacity.
 * @param num Initial capacity (if < 1, a default capacity is used).
 * @return Pointer to the new DataVector, or NULL on allocation failure.
 */
DataVector *DataVectorCreate(size_t num);

/**
 * @brief Destroys a DataVector and frees all associated memory.
 * @param v Pointer to the DataVector (can be NULL).
 */
void DataVectorDestroy(DataVector *v);

/* ----- Modification ----- */

/**
 * @brief Appends a copy of a StatData element to the end of the vector.
 * @param v Pointer to the DataVector.
 * @param data Pointer to the StatData to add.
 * @return 0 on success, -1 on memory allocation failure.
 */
int DataVectorPush(DataVector *v, const StatData *data);

/**
 * @brief Replaces the internal data array with a new one, taking ownership.
 *
 * The previous data array is freed. The vector assumes ownership of `data`.
 *
 * @param v Pointer to the DataVector.
 * @param data Pointer to the new StatData array (must be allocated with
 * malloc).
 * @param size Number of elements in the array.
 * @return 0 on success, -1 if invalid arguments.
 */
int DataVectorSetData(DataVector *v, StatData *data, size_t size);

/**
 * @brief Removes the last element and optionally returns its value.
 * @param v Pointer to the DataVector.
 * @param out If not NULL, receives the removed element.
 * @return 0 on success, -1 if the vector is empty.
 */
int DataVectorPop(DataVector *v, StatData *out);