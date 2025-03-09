#include <pthread.h>
#include <stdbool.h>
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
  size_t core;
} PartitionArgs;

void make_buffers(Partition *partitions, size_t n_partitions, size_t n_tuples,
                  size_t n_threads) {
  for (size_t i = 0; i < n_partitions; i++) {
    Buffer *buffer = malloc(sizeof(Buffer));

    buffer->size = n_tuples / n_partitions * 2;
    buffer->tuples = malloc(buffer->size * sizeof(Tuple));
    buffer->current_writing_index = 0;

    partitions[i].n_buffers = 1;
    partitions[i].buffers = buffer;
  }
}

void free_buffers(Partition *partitions, size_t n_partitions, size_t n_tuples, size_t n_threads) {
  for (size_t p = 0; p < n_partitions; p++) {
    free(partitions[p].buffers->tuples);
    free(partitions[p].buffers);
  }
}

void *partition_part(void *args) {
  PartitionArgs *p_args = args;
  Partition *partitions = p_args->partitions;
  size_t n_partitions = p_args->n_partitions;
  Tuple *tuples = p_args->tuples;
  size_t n_tuples = p_args->n_tuples;
  size_t hash_bits = p_args->hash_bits;
  size_t core = p_args->core;

  // Affinity defined in cmake file, to allow compiling with and without setting
  // core affinity
#ifdef AFFINITY
  cpu_set_t mask;
  CPU_ZERO(&mask);
  CPU_SET(core, &mask);
  int result = sched_setaffinity(
      0, sizeof(mask), &mask); // 0 means current thread. Returns 0 on succes
#endif

  for (size_t i = 0; i < n_tuples; i++) {
    Tuple *tuple = &tuples[i];
    size_t partition = hash(tuple, hash_bits);
    size_t write_idx = atomic_fetch_add(&partitions[partition].buffers->current_writing_index, 1);
    partitions[partition].buffers->tuples[write_idx] = *tuple;
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
#ifdef BAD_AFFINITY
    partition_args[t].core = t; // For demonstratitive purposes
                                // Used to show that crossing NUMA nodes hurts performance
#else
    partition_args[t].core =
        t * 2 % 32 +
        t / 16; // First fill first numa node, then use two nodes
                // Designed for a processor with 32 threads and two numa nodes
                // with even cores on one and odd on the other
#endif

    pthread_create(&threads[t], NULL, partition_part, &partition_args[t]);
  }

  for (int t = 0; t < n_threads; t++) {
    pthread_join(threads[t], NULL);
  }

  free(partition_args);
}

bool contains_tuple(Partition *partition, Tuple *tuple) {
  for (size_t t = 0; t < partition->buffers->size; t++) {
    if (tuple->key == partition->buffers->tuples[t].key)
      return true;
  }

  return false;
}
