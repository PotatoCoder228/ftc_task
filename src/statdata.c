#include "statdata.h"

/* Геттеры */
long StatDataGetId(const StatData *sd) { return sd->id; }

int StatDataGetCount(const StatData *sd) { return sd->count; }

float StatDataGetCost(const StatData *sd) { return sd->cost; }

unsigned int StatDataGetPrimary(const StatData *sd) { return sd->primary; }

unsigned int StatDataGetMode(const StatData *sd) { return sd->mode; }

/* Сеттеры */
void StatDataSetId(StatData *sd, long id) { sd->id = id; }

void StatDataSetCount(StatData *sd, int count) { sd->count = count; }

void StatDataSetCost(StatData *sd, float cost) { sd->cost = cost; }

void StatDataSetPrimary(StatData *sd, unsigned int primary) {
  sd->primary = primary;
}

void StatDataSetMode(StatData *sd, unsigned int mode) { sd->mode = mode; }

/* Инициализация */
void StatDataInit(StatData *sd, long id, int count, float cost,
                  unsigned int primary, unsigned int mode) {
  sd->id = id;
  sd->count = count;
  sd->cost = cost;
  sd->primary = primary;
  sd->mode = mode;
}

/* Компараторы */
int StatDataCompareByCost(const void *a, const void *b) {
  const StatData *sa = (const StatData *)a;
  const StatData *sb = (const StatData *)b;
  return (sa->cost > sb->cost) - (sa->cost < sb->cost);
}

int StatDataCompareById(const StatData *a, const StatData *b) {
  return (a->id > b->id) - (a->id < b->id);
}

int LongCompare(long a, long b) { return (a > b) - (a < b); }