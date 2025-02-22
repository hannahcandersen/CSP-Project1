#include <stdlib.h>
#include <pthread.h>
#include <stdatomic.h>

#include "partition.h"
#define NO_CHUNK ((size_t)-1)

typedef struct {
  size_t id;
  Partition *partitions; 
  size_t n_partitions;   
  Tuple *tuples;          // whole array of tuples
  size_t start_of_slice;  // start of the slice of tuples
  size_t end_of_slice;    // end of the slice of tuples
  size_t hash_bits;      
} PartitionWorkerArgs;

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

void *partition_worker(void *args)
{
  PartitionWorkerArgs *pwa = (PartitionWorkerArgs *)args;

  size_t id = pwa->id;
  Partition *partitions = pwa->partitions;
  size_t n_partitions = pwa->n_partitions;
  Tuple *tuples = pwa->tuples;
  size_t start_of_slice = pwa->start_of_slice;
  size_t end_of_slice = pwa->end_of_slice;
  size_t hash_bits = pwa->hash_bits;

  size_t current_chunk[n_partitions];
  for (size_t i = 0; i < n_partitions; i++)
  {
    current_chunk[i] = NO_CHUNK;
  }

  // Loop through the slice of tuples
  for (size_t i = start_of_slice; i < end_of_slice; i++)
  {
    Tuple *tuple = &tuples[i];

    // Hash tuple to find partition and output buffer
    size_t hash_value = hash(tuple, hash_bits);
    size_t partition_id = hash_value;
    Partition *partition = &partitions[partition_id];
    Buffer *buffer = &partition->buffers[0];

    // Check if chunk has been claimed for this buffer
    if (current_chunk[partition_id] == NO_CHUNK)
    {
      // Claim a chunk and increment h
      current_chunk[partition_id] = atomic_fetch_add(&buffer->h, 1);
    }

    // If no space in current chunk, claim a new chunk.
    if (buffer->d[id] >= buffer->c)
    {
      current_chunk[partition_id] = atomic_fetch_add(&buffer->h, 1);
      buffer->d[id] = 0;
    }

    // Write tuple to chunk in buffer
    size_t offset = current_chunk[partition_id] * buffer->c + buffer->d[id];
    buffer->tuples[offset] = *tuple;
    buffer->d[id]++;

    // TODO: If neccessary
    // If all tuples are processed or buffer is full, set finalized to true
    // Topup after finalization
  }

  return NULL;
}

void partition(Partition *partitions, size_t n_partitions, Tuple *tuples, size_t n_tuples, size_t hash_bits, size_t n_threads) {

  // TODO: Fix slices to not miss any if n_tuples is not perfectly divisible by n_threads
  size_t slice = n_tuples / n_threads; // Size of slices that the input array will be divided into

  pthread_t *threads = malloc(n_threads * sizeof(pthread_t));
  PartitionWorkerArgs *thread_args = malloc(n_threads * sizeof(PartitionWorkerArgs));

  for (size_t i = 0; i < n_threads; i++) {
    thread_args[i].id = i;
    thread_args[i].partitions = partitions;
    thread_args[i].n_partitions = n_partitions;
    thread_args[i].tuples = tuples;
    thread_args[i].start_of_slice = i * slice;
    thread_args[i].end_of_slice = (i + 1) * slice;
    thread_args[i].hash_bits = hash_bits;

    pthread_create(&threads[i], NULL, partition_worker, &thread_args[i]);
  }

  for (size_t t = 0; t < n_threads; t++) {
    pthread_join(threads[t], NULL);
  }

  free(threads);
  free(thread_args);
}