#include "tuple.h"
#include <stdbool.h>

# ifndef PARTITION
# define PARTITION

typedef struct {
  Tuple *tuples;
  size_t size;
  size_t p;               // number of threads
  _Atomic size_t h;       // next available chunk index
  size_t *d;              // per-thread occupancy counters
  _Atomic bool finalized; // flag indicating the buffer has entered finalization
} Buffer;

typedef struct {
  Buffer *buffers;
  size_t n_buffers;
} Partition;

void make_buffers(Partition *partitions, size_t n_partitions, size_t n_tuples);
void partition(Partition *partitions, size_t n_partitions, Tuple *tuples, size_t n_tuples, size_t hash_bits, size_t n_threads);

# endif
