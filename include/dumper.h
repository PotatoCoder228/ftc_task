#pragma once
#include <stdlib.h>
#include <stdint.h>

// typedef struct StatData StatData;

typedef struct StatData {
  long id;
  int count;
  float cost;
  unsigned int primary : 1;
  unsigned int mode : 3;
} StatData;

typedef struct DataVector DataVector;

StatData DataVectorGet(DataVector* v, size_t i);
size_t DataVectorSize(DataVector* v);

int StoreDump(const char* name, const DataVector* arr);

DataVector *LoadDump(const char *name);

DataVector* JoinDump(const DataVector* dst, const DataVector* src);

int SortDump(DataVector *base, int (*compare)(const void*, const void*));

DataVector *DataVectorCreate(size_t num);

int DataVectorPush(DataVector *vector, StatData *data);

int DataVectorSetData(DataVector *vector, StatData *data, size_t size);

int DataVectorPop(DataVector *vector, StatData *data);

void DataVectorDestroy(DataVector *vector);