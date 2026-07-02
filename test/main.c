#include <test.h>



int main() {
  testInvalidArgsAll();
  testInvalidArgs1();
  testInvalidArgs2();
  testInputNotExists();
  testOutputExists();
  testOutputPermissionDenied();
  testInputPermissionDenied();
  testSuccess();
  return 0;
}
