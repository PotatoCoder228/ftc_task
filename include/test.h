#pragma once
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <test_data.h>

int runProcess(const char* cmd);

bool testInvalidArgsAll();

bool testInvalidArgs1();
bool testInvalidArgs2();
bool testInputNotExists();
bool testOutputExists();

bool testOutputPermissionDenied();
bool testInputPermissionDenied();
bool testSuccess();