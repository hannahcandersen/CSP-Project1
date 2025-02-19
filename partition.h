#include "tuple.h"

# ifndef PARTITION
# define PARTITION

typedef struct {
  Tuple *tuples;
  size_t size;
} Buffer;

typedef struct {
  Buffer *buffers;
  size_t n_buffers;
} Partition;

void make_buffers(Partition *partitions, size_t n_partitions, size_t n_tuples);
void partition(Partition *partitions, size_t n_partitions, Tuple *tuples, size_t n_tuples, size_t hash_bits, size_t n_threads);

# endif
