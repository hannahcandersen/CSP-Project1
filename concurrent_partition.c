#include <pthread.h>
#include <stdatomic.h>
#include <stdlib.h>

#include "hash.h"
#include "partition.h"

typedef struct {
  Partition *partitions;
  size_t n_partitions;
  Tuple *tuples;
  size_t n_tuples;
  size_t hash_bits;
} PartitionArgs;

void make_buffers(Partition *partitions, size_t n_partitions, size_t n_tuples,
                  size_t n_threads) {
  for (size_t i = 0; i < n_partitions; i++) {
    Buffer *buffer = malloc(sizeof(Buffer));

    buffer->size = n_tuples / n_partitions * 2;
    buffer->tuples = malloc(buffer->size * sizeof(Tuple));

    partitions[i].n_buffers = 1;
    partitions[i].buffers = buffer;
  }
}

void *partition_part(void *args) {
  PartitionArgs *p_args = args;
  Partition *partitions = p_args->partitions;
  size_t n_partitions = p_args->n_partitions;
  Tuple *tuples = p_args->tuples;
  size_t n_tuples = p_args->n_tuples;
  size_t hash_bits = p_args->hash_bits;

  for (int i = 0; i < n_tuples; i++) {
    size_t partition = hash(&tuples[i], hash_bits);
    size_t buffer_idx = atomic_fetch_add(&partitions[partition].buffers->current_writing_index, 1);
    partitions[partition].buffers->tuples[buffer_idx] = tuples[i];
  }

  return NULL;
}

void partition(Partition *partitions, size_t n_partitions, Tuple *tuples,
               size_t n_tuples, size_t hash_bits, size_t n_threads) {
  pthread_t threads[n_threads];

  size_t tuples_per_thread = n_tuples / n_threads;

  PartitionArgs *partition_args = malloc(n_threads * sizeof(PartitionArgs));

  for (int t = 0; t < n_threads; t++) {
    partition_args[t].partitions = partitions;
    partition_args[t].n_partitions = n_partitions;
    partition_args[t].tuples = &tuples[t * tuples_per_thread];
    partition_args[t].n_tuples = tuples_per_thread;
    partition_args[t].hash_bits = hash_bits;

    pthread_create(&threads[t], NULL, partition_part, &partition_args[t]);
  }

  for (int t = 0; t < n_threads; t++) {
    pthread_join(threads[t], NULL);
  }

  free(partition_args);
}
