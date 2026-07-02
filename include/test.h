#pragma once

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <test_data.h>

/**
 * @brief Number of iterations for each test to measure average time.
 */
static const int ITERATIONS = 10;

/**
 * @brief Executes a shell command and captures its exit code.
 *
 * This function runs the given command via popen, reads and discards its output
 * to avoid SIGPIPE, and returns the exit status of the command.
 *
 * @param cmd The command string to execute.
 * @return The exit code of the command (0 on success, >0 on error),
 *         or -1 if an error occurred (e.g., command failed, popen/pclose error,
 *         process terminated by signal).
 */
int runProcess(const char *cmd);

/**
 * @brief Prints a formatted test result with the test name, success status, and
 * average time.
 * @param fn Name of the test function (usually __func__).
 * @param res True if the test passed, false otherwise.
 * @param time Average execution time per iteration (in seconds).
 */
void printTestResult(const char *fn, bool res, double time);

/* ----- Test functions (each returns true if all iterations succeeded) ----- */

/**
 * @brief Tests invalid argument handling (no arguments).
 * @return true if the program exits with non-zero status, false otherwise.
 */
bool testInvalidArgsAll(void);

/**
 * @brief Tests invalid argument handling (only one argument).
 * @return true if the program exits with non-zero status, false otherwise.
 */
bool testInvalidArgs1(void);

/**
 * @brief Tests invalid argument handling (two arguments).
 * @return true if the program exits with non-zero status, false otherwise.
 */
bool testInvalidArgs2(void);

/**
 * @brief Tests behavior when an input file does not exist.
 * @return true if the program exits with non-zero status, false otherwise.
 */
bool testInputNotExists(void);

/**
 * @brief Tests behavior when the output file already exists (should fail).
 * @return true if the program exits with non-zero status, false otherwise.
 */
bool testOutputExists(void);

/**
 * @brief Tests behavior when the output file has no write permission.
 * @return true if the program exits with non-zero status, false otherwise.
 */
bool testOutputPermissionDenied(void);

/**
 * @brief Tests behavior when an input file has no read permission.
 * @return true if the program exits with non-zero status, false otherwise.
 */
bool testInputPermissionDenied(void);

/**
 * @brief Tests a successful run: merges two input dumps and verifies the
 * output.
 * @return true if the output matches the expected result, false otherwise.
 */
bool testSuccess(void);

/**
 * @brief Prepares test input files by writing two arrays of StatData to disk.
 *
 * This function allocates memory, copies the given arrays, and stores them
 * using StoreDump. It is intended for use in test setups.
 *
 * @param fn1 Filename for the first dump.
 * @param fn2 Filename for the second dump.
 * @param arr1 Pointer to the first array of StatData.
 * @param arr2 Pointer to the second array of StatData.
 * @param num1 Number of elements in arr1.
 * @param num2 Number of elements in arr2.
 */
static inline void StoreTestSet(const char *fn1, const char *fn2,
                                StatData *arr1, StatData *arr2, size_t num1,
                                size_t num2) {
  if (!fn1 || !fn2 || !arr1 || !arr2) {
    return;
  }
  StatData *data1 = malloc(sizeof(StatData) * num1);
  StatData *data2 = malloc(sizeof(StatData) * num2);

  memcpy(data1, arr1, sizeof(StatData) * num1);
  memcpy(data2, arr2, sizeof(StatData) * num2);

  DataVector *v = DataVectorCreate(0);
  DataVectorSetData(v, data1, num1);
  StoreDump(fn1, v);
  DataVectorSetData(v, data2, num2);
  StoreDump(fn2, v);
  DataVectorDestroy(v);
}