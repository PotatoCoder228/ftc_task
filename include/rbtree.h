#pragma once

#include "stdint.h"
#include <stdbool.h>

/**
 * @brief Color constants for Red-Black Tree nodes.
 */
static const int8_t BLACK = 0;
static const int8_t RED = 1;

typedef void *(*Callback)(void *data);

/**
 * @brief Opaque type for Red-Black Tree node.
 */
typedef struct RbTree RbTree;

/* ----- Node creation and destruction ----- */

/**
 * @brief Creates a new Red-Black Tree node with the given key.
 * @param key The key to store.
 * @return Pointer to the new node, or NULL on allocation failure.
 */
RbTree *NewRbNode(long key);

/**
 * @brief Recursively destroys an entire Red-Black Tree.
 * @param node Root of the tree (or NULL).
 */
void RbDestroy(void *node);

/**
 * @brief Checks if a node is empty (NULL).
 * @param node Pointer to the node.
 * @return true if node is NULL, false otherwise.
 */
bool RbIsEmpty(RbTree *node);

/* ----- Getters and setters ----- */

void RbSetColor(RbTree *node, int8_t color);
void RbSetKey(RbTree *node, long key);
void RbSetP(RbTree *node, RbTree *parent);
void RbSetLeft(RbTree *node, RbTree *left);
void RbSetRight(RbTree *node, RbTree *right);

/**
 * @brief Returns the key of a node.
 * @param node Pointer to the node.
 * @return The key, or 0 if node is NULL.
 */
long RbKey(RbTree *node);

/**
 * @brief Returns the color of a node.
 * @param node Pointer to the node.
 * @return BLACK or RED, or BLACK if node is NULL.
 */
int8_t RbColor(RbTree *node);

/**
 * @brief Returns the parent of a node.
 * @param node Pointer to the node.
 * @return Parent node, or NULL.
 */
RbTree *RbParent(RbTree *node);

/**
 * @brief Returns the left child of a node.
 * @param node Pointer to the node.
 * @return Left child, or NULL.
 */
RbTree *RbLeft(RbTree *node);

/**
 * @brief Returns the right child of a node.
 * @param node Pointer to the node.
 * @return Right child, or NULL.
 */
RbTree *RbRight(RbTree *node);

/* ----- Search and order statistics ----- */

/**
 * @brief Searches for a key in the Red-Black Tree.
 * @param node Root of the tree.
 * @param target Key to search for.
 * @param comparator Function to compare two long values (returns -1, 0, 1).
 * @return Pointer to the node with the key, or NULL if not found.
 */
RbTree *RbSearch(RbTree *node, long target, int comparator(long, long));

/**
 * @brief Finds the node with the maximum key in a subtree.
 * @param node Root of the subtree (must not be NULL).
 * @return Node with maximum key.
 */
RbTree *RbMax(RbTree *node);

/**
 * @brief Finds the node with the minimum key in a subtree.
 * @param node Root of the subtree (must not be NULL).
 * @return Node with minimum key.
 */
RbTree *RbMin(RbTree *node);

/**
 * @brief Returns the in-order successor of a given node.
 * @param node The node.
 * @return Successor node, or NULL if none.
 */
RbTree *RbSuccessor(RbTree *node);

/**
 * @brief Returns the in-order predecessor of a given node.
 * @param node The node.
 * @return Predecessor node, or NULL if none.
 */
RbTree *RbPredecessor(RbTree *node);

/* ----- Insertion and deletion ----- */

/**
 * @brief Inserts a key into the Red-Black Tree.
 * @param root Pointer to the root pointer (may be modified).
 * @param v Key to insert.
 * @param comparator Comparator function.
 * @return true if inserted successfully, false if key already exists or error.
 */
bool RbInsert(RbTree **root, long v, int comparator(long, long));

/**
 * @brief Deletes a node from the Red-Black Tree.
 * @param root Pointer to the root pointer.
 * @param z Node to delete (must be a valid node in the tree).
 * @return true if deletion succeeded, false otherwise.
 */
bool RbDelete(RbTree **root, RbTree *z);

/* ----- Iterators ----- */

/**
 * @brief Returns the first node (minimum) of the tree.
 * @param root Root of the tree.
 * @return Minimum node, or NULL.
 */
RbTree *RbStart(RbTree *root);

/**
 * @brief Checks if a node has a successor.
 * @param node The node.
 * @return true if a successor exists.
 */
bool RbHasNext(RbTree *node);

/**
 * @brief Checks if a node has a predecessor.
 * @param node The node.
 * @return true if a predecessor exists.
 */
bool RbHasPrev(RbTree *node);

/**
 * @brief Returns the successor of a node (same as RbSuccessor).
 * @param node The node.
 * @return Successor node.
 */
RbTree *RbPrev(RbTree *node);

/**
 * @brief Returns the predecessor of a node (same as RbPredecessor).
 * @param node The node.
 * @return Predecessor node.
 */
RbTree *RbNext(RbTree *node);

/**
 * @brief Returns the last node (maximum) of the tree.
 * @param root Root of the tree.
 * @return Maximum node, or NULL.
 */
RbTree *RbEnd(RbTree *root);
