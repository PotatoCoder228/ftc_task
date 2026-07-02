#include "dumper.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void printBinary(unsigned int n) {
  if (n == 0) {
    putchar('0');
    return;
  }
  unsigned int mask = 1u << (sizeof(n) * 8 - 1);
  while (mask && !(n & mask))
    mask >>= 1; // skip leading zeros
  while (mask) {
    putchar((n & mask) ? '1' : '0');
    mask >>= 1;
  }
}

void DataVectorPrint(DataVector *v) {
  size_t size = DataVectorSize(v);
  printf("id\tcount\tcost\t\tprimary\t\tmode\n");

  printf("--------------------------------------------------------\n");

  for (size_t i = 0; i < 10; i++) {
    StatData data = DataVectorGet(v, i);
    printf("%#lx\t%d\t%+.3e\t%s\t\t", data.id, data.count, data.cost,
           data.primary ? "y" : "n");
    printBinary(data.mode);
    putchar('\n');
  }
}

int main(int argc, char *argv[]) {
  // Проверка количества аргументов
  if (argc != 4) {
    char msg[256];
    snprintf(msg, sizeof(msg), "Usage: %s <filename1> <filename2>", argv[0]);
    errno = EINVAL;
    perror(msg);
    return EXIT_FAILURE;
  }

  const char *fn1 = argv[1];
  const char *fn2 = argv[2];
  const char *dstFn = argv[3];

  DataVector *v1 = LoadDump(fn1);
  DataVector *v2 = LoadDump(fn2);
  DataVector *vRes = JoinDump(v1, v2);

  if (SortDump(vRes, NULL) != 0) {
    perror("Dump is not sorted!");
    return EXIT_FAILURE;
  }
  DataVectorPrint(vRes);
  if (StoreDump(dstFn, vRes) != 0) {
    perror("Dump is not saved!");
    return EXIT_FAILURE;
  }
  return 0;
}
