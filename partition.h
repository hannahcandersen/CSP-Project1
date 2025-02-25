#include "tuple.h"
#include <stdbool.h>

# ifndef PARTITION
# define PARTITION

typedef struct {
  Tuple *tuples;
  size_t size;
  size_t c;            // Chunk size
  _Atomic size_t h;    // Next available chunk index
  size_t *d;           // How many tuples have been written into the current chunk by thread i.
  _Atomic size_t current_writing_index;
} Buffer;

typedef struct {
  Buffer *buffers;
  size_t n_buffers;
} Partition;

void make_buffers(Partition *partitions, size_t n_partitions, size_t n_tuples, size_t n_threads);
void partition(Partition *partitions, size_t n_partitions, Tuple *tuples, size_t n_tuples, size_t hash_bits, size_t n_threads);
bool contains_tuple(Partition *partition, Tuple *tuple);

# endif
