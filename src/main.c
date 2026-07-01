#include "dumper.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Функция для вывода числа в двоичном виде (без ведущих нулей)
void printBinary(unsigned int n) {
    if (n == 0) {
        putchar('0');
        return;
    }
    // Находим старший значащий бит
    unsigned int mask = 1u << (sizeof(n) * 8 - 1);
    while (mask && !(n & mask)) mask >>= 1; // пропускаем ведущие нули
    while (mask) {
        putchar((n & mask) ? '1' : '0');
        mask >>= 1;
    }
}

void DataVectorPrint(DataVector* v){
  printf("Print vector with size %ld\n", DataVectorSize(v));
  for(size_t i = 0; i < DataVectorSize(v); i++){
    StatData data = DataVectorGet(v, i);
    printf("id:      %#lx ", data.id);                    // 0x1a3f
    printf("count:   %d ", data.count);                 // 12345
    printf("cost:    %+.3e ", data.cost);               // +1.230e+02
    printf("primary: %s ", data.primary ? "y" : "n");   // y
    printf("mode:    ");
    printBinary(data.mode);                             // 101
    putchar('\n');
  }
}

int main(int argc, char *argv[]) {
  // Проверка количества аргументов
  if (argc != 4) {
    char msg[256];
    snprintf(msg, sizeof(msg), "Usage: %s <string1> <string2>", argv[0]);
    errno = EINVAL;
    perror(msg);
    return EXIT_FAILURE;
  }

  // Получаем указатели на строки
  const char *fn1 = argv[1];
  const char *fn2 = argv[2];
  const char *dstFn = argv[2];

  DataVector* v1 = LoadDump(fn1);
  DataVector* v2 = LoadDump(fn2);
  DataVector* vRes = JoinDump(v1, v2);
  printf("vRes\n");
  DataVectorPrint(vRes);
  SortDump(vRes, NULL);
  printf("vRes\n");
  DataVectorPrint(vRes);
  StoreDump(dstFn,vRes);
  return 0;
}

// StatData case_1_in_a[2] = {
//     {.id = 90889, .count = 13, .cost = 3.567, .primary = 0, .mode = 3},
//     {.id = 90089, .count = 1, .cost = 88.90, .primary = 1, .mode = 0}};
// StatData case_1_in_b[2] = {
//     {.id = 90089, .count = 13, .cost = 0.011, .primary = 0, .mode = 2},
//     {.id = 90189, .count = 1000, .cost = 1.00003, .primary = 1, .mode = 2}};
// /* Ожидаемый результат обработки */
// StatData case_1_out[3] = {
//     {.id = 90189, .count = 1000, .cost = 1.00003, .primary = 1, .mode = 2},
//     {.id = 90889, .count = 13, .cost = 3.567, .primary = 0, .mode = 3},
//     {.id = 90089, .count = 14, .cost = 88.911, .primary = 0, .mode = 2}};

// int main() {
//   StatData *data1 = malloc(sizeof(StatData) * 2);
//   StatData *data2 = malloc(sizeof(StatData) * 2);

//   memcpy(data1, case_1_in_a, 2*sizeof(StatData));
//   memcpy(data2, case_1_in_b, 2*sizeof(StatData));

//   DataVector *v = DataVectorCreate(0);
//   DataVectorSetData(v, data1, 2);
//   StoreDump("aboba1.dump", v);
//   DataVectorSetData(v, data2, 2);
//   StoreDump("aboba2.dump", v);
//   return 0;
// }