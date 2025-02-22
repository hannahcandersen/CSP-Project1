#include "hash.h"

size_t hash(Tuple *tuple, size_t hash_bits) {
  unsigned long long int A = 0x9E3779B97F4A7C15llu;
  return (tuple->key * A) >> (64 - hash_bits);
}