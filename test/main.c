#include "dumper.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <test.h>
#include "test_data.h"

int main() {
  if(!testEmptyFilenames()){
    printf("testEmptyFilenames failed");
  }
  else{
    printf("testEmptyFilenames success");
  }
  return 0;
}
