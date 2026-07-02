#pragma once

#define SafeRelease(p)                                                         \
  do {                                                                         \
    free(p);                                                                   \
    (p) = NULL;                                                                \
  } while (0)