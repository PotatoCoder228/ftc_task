#pragma once

#include <stddef.h>

/**
 * @brief Structure representing a single statistical data record.
 *
 * Fields:
 * - id: unique identifier (long)
 * - count: integer count
 * - cost: floating-point cost
 * - primary: 1-bit flag (0 or 1)
 * - mode: 3-bit value (0-7)
 */
typedef struct StatData {
  long id;
  int count;
  float cost;
  unsigned int primary : 1;
  unsigned int mode : 3;
} StatData;

/* ----- Getters ----- */

/**
 * @brief Returns the id field of a StatData structure.
 * @param sd Pointer to the StatData structure.
 * @return The id value.
 */
long StatDataGetId(const StatData *sd);

/**
 * @brief Returns the count field.
 * @param sd Pointer to the StatData structure.
 * @return The count value.
 */
int StatDataGetCount(const StatData *sd);

/**
 * @brief Returns the cost field.
 * @param sd Pointer to the StatData structure.
 * @return The cost value.
 */
float StatDataGetCost(const StatData *sd);

/**
 * @brief Returns the primary flag.
 * @param sd Pointer to the StatData structure.
 * @return 1 if primary is set, 0 otherwise.
 */
unsigned int StatDataGetPrimary(const StatData *sd);

/**
 * @brief Returns the mode field.
 * @param sd Pointer to the StatData structure.
 * @return The mode value (0-7).
 */
unsigned int StatDataGetMode(const StatData *sd);

/* ----- Setters ----- */

/**
 * @brief Sets the id field.
 * @param sd Pointer to the StatData structure.
 * @param id New id value.
 */
void StatDataSetId(StatData *sd, long id);

/**
 * @brief Sets the count field.
 * @param sd Pointer to the StatData structure.
 * @param count New count value.
 */
void StatDataSetCount(StatData *sd, int count);

/**
 * @brief Sets the cost field.
 * @param sd Pointer to the StatData structure.
 * @param cost New cost value.
 */
void StatDataSetCost(StatData *sd, float cost);

/**
 * @brief Sets the primary flag.
 * @param sd Pointer to the StatData structure.
 * @param primary New primary value (0 or 1).
 */
void StatDataSetPrimary(StatData *sd, unsigned int primary);

/**
 * @brief Sets the mode field.
 * @param sd Pointer to the StatData structure.
 * @param mode New mode value (0-7).
 */
void StatDataSetMode(StatData *sd, unsigned int mode);

/* ----- Initialization ----- */

/**
 * @brief Initializes all fields of a StatData structure at once.
 * @param sd Pointer to the StatData structure.
 * @param id Id value.
 * @param count Count value.
 * @param cost Cost value.
 * @param primary Primary flag.
 * @param mode Mode value.
 */
void StatDataInit(StatData *sd, long id, int count, float cost,
                  unsigned int primary, unsigned int mode);

/* ----- Comparators ----- */

/**
 * @brief Compares two StatData structures by their cost field.
 * @param a Pointer to first StatData (as const void*).
 * @param b Pointer to second StatData (as const void*).
 * @return Negative if a->cost < b->cost, positive if >, zero if equal.
 */
int StatDataCompareByCost(const void *a, const void *b);

/**
 * @brief Compares two StatData structures by their id field.
 * @param a Pointer to first StatData.
 * @param b Pointer to second StatData.
 * @return Negative if a->id < b->id, positive if >, zero if equal.
 */
int StatDataCompareById(const StatData *a, const StatData *b);

/**
 * @brief Compares two long integers.
 * @param a First long value.
 * @param b Second long value.
 * @return 1 if a > b, -1 if a < b, 0 if equal.
 */
int LongCompare(long a, long b);
