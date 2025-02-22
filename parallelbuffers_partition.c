#include <stdlib.h>

#include "partition.h"

void make_buffers(Partition *partitions, size_t n_partitions, size_t n_tuples, size_t n_threads) {
  for (size_t i = 0; i < n_partitions; i++) {
    Buffer *buffer = malloc(sizeof(Buffer));

    buffer->size = n_tuples / n_partitions * 2;
    buffer->tuples = malloc(buffer->size * sizeof(Tuple));
    buffer->c = 1024; // TODO: Change? Experiment with different values
    buffer->h = 0;
    buffer->d = malloc(n_threads * sizeof(size_t));
    for (size_t j = 0; j < n_threads; j++) {
      buffer->d[j] = 0;
    }
    buffer->finalized = false;

    partitions[i].n_buffers = 1;
    partitions[i].buffers = buffer;
  }
}

// TODO: Implement
void partition(Partition *partitions, size_t n_partitions, Tuple *tuples, size_t n_tuples, size_t hash_bits, size_t n_threads) {

}
