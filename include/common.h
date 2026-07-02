#pragma once

/**
 * @brief Safely releases dynamically allocated memory and nullifies the
 * pointer.
 *
 * This macro frees the memory pointed to by `p` and sets `p` to NULL to prevent
 * use-after-free errors. It is intended for use with pointers that may be
 * reused.
 *
 * @param p Pointer to the memory block to free. Must be a valid pointer or
 * NULL.
 */
#define SafeRelease(p)                                                         \
  do {                                                                         \
    free(p);                                                                   \
    (p) = NULL;                                                                \
  } while (0)
