#include <math.h>

#include "tuple.h"

#ifndef HASH
#define HASH

size_t hash(Tuple *tuple, size_t n_partitions) {
  unsigned long long int A = 0x9E3779B97F4A7C15;
  return ((tuple->key * A) >> (64 - (unsigned long long int)log2((double)n_partitions))) % n_partitions;
}

#endif
