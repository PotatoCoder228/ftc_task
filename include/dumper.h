#pragma once

#include "datavector.h"
#include "statdata.h"

/**
 * @brief Writes a DataVector to a binary file.
 * @param name Filename.
 * @param arr DataVector to store.
 * @return 0 on success, -1 on error (errno set).
 */
int StoreDump(const char *name, const DataVector *arr);

/**
 * @brief Reads a binary file and creates a DataVector from its contents.
 * @param name Filename.
 * @return Pointer to a new DataVector, or NULL on error (errno set).
 */
DataVector *LoadDump(const char *name);

/**
 * @brief Merges two DataVectors, combining records with same id.
 *
 * For records with matching id:
 * - count and cost are summed.
 * - primary = logical AND (1 only if both are 1).
 * - mode = maximum of the two.
 *
 * The resulting vector is sorted by cost (ascending) after merge.
 *
 * @param v1 First DataVector.
 * @param v2 Second DataVector.
 * @return New DataVector containing the merged result, or NULL on error.
 */
DataVector *JoinDump(const DataVector *v1, const DataVector *v2);

/**
 * @brief Sorts a DataVector in-place using the given comparison function.
 * @param base DataVector to sort.
 * @param compare Comparison function (takes two const void*).
 *               If NULL, default comparison by cost is used.
 * @return 0 on success, -1 on error.
 */
int SortDump(DataVector *base, int (*compare)(const void *, const void *));
