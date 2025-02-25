#include <stdlib.h>
#include <pthread.h>
#include <stdatomic.h>

#include "partition.h"
#include "hash.h"
#define NO_CHUNK ((size_t)-1)
#define CHUNK_SIZE 2 // TODO: Change to big number. How to find the optimal value?

typedef struct {
  size_t id;
  Partition *partitions; 
  size_t n_partitions;   
  Tuple *tuples;          // All tuples
  size_t start_of_slice;  // Start of thread's slice of tuples
  size_t end_of_slice;    // End of thread's slice of tuples
  size_t hash_bits;      
} PartitionWorkerArgs;

void make_buffers(Partition *partitions, size_t n_partitions, size_t n_tuples, size_t n_threads) {
  for (size_t i = 0; i < n_partitions; i++) {
    Buffer *buffer = malloc(sizeof(Buffer));

    buffer->size = n_tuples / n_partitions * 2;
    buffer->tuples = malloc(buffer->size * sizeof(Tuple));
    buffer->c = CHUNK_SIZE; 
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

  printf("Thread %zu started. Has been assigned tuples no. %zu - %zu \n", id, start_of_slice, end_of_slice - 1);

  size_t *current_chunk = malloc(n_partitions * sizeof(size_t));  
  for (size_t i = 0; i < n_partitions; i++)
  {
    current_chunk[i] = NO_CHUNK;
  }

  for (size_t i = start_of_slice; i < end_of_slice; i++)
  {
    Tuple *tuple = &tuples[i];
    size_t hash_value = hash(tuple, hash_bits);
    size_t partition_id = hash_value;
    Partition *partition = &partitions[partition_id];
    Buffer *buffer = &partition->buffers[0];

    printf("Thread %zu is processing tuple %zu. Hash value: %zu. Partition: %zu\n", id, i, hash_value, partition_id);

    if (current_chunk[partition_id] == NO_CHUNK)
    {
      printf("Thread %zu got its first chunk for partition %zu\n", id, partition_id);
      current_chunk[partition_id] = atomic_fetch_add(&buffer->h, 1);
    }

    if (buffer->d[id] >= buffer->c)
    {
      printf("Thread %zu has filled its chunk for partition %zu and is claiming a new one\n", id, partition_id);
      current_chunk[partition_id] = atomic_fetch_add(&buffer->h, 1);
      buffer->d[id] = 0;
    }

    size_t offset = current_chunk[partition_id] * buffer->c + buffer->d[id];
    buffer->tuples[offset] = *tuple;
    buffer->d[id]++;
    printf("Thread %zu has written tuple no. %zu to partition %zu at offset %zu\n", id, i, partition_id, offset);

    // TODO: If neccessary
    // If all tuples are processed or buffer is full, set finalized to true
    // Topup after finalization. To avoid "holes" in the output
  }

  free(current_chunk);
  return NULL;
}

void partition(Partition *partitions, size_t n_partitions, Tuple *tuples, size_t n_tuples, size_t hash_bits, size_t n_threads) {

  size_t slice = n_tuples / n_threads; 
  printf("Each thread will process %zu tuples\n", slice);

  pthread_t *threads = malloc(n_threads * sizeof(pthread_t));
  PartitionWorkerArgs *thread_args = malloc(n_threads * sizeof(PartitionWorkerArgs));

  for (size_t i = 0; i < n_threads; i++) {
    thread_args[i].id = i;
    thread_args[i].partitions = partitions;
    thread_args[i].n_partitions = n_partitions;
    thread_args[i].tuples = tuples;
    thread_args[i].start_of_slice = i * slice;
    thread_args[i].end_of_slice = (i == n_threads - 1) ? n_tuples + 1 : (i + 1) * slice; // Last thread gets the rest
    thread_args[i].hash_bits = hash_bits;

    pthread_create(&threads[i], NULL, partition_worker, &thread_args[i]);
  }

  for (size_t t = 0; t < n_threads; t++) {
    pthread_join(threads[t], NULL);
    printf("Thread %zu finished\n", t);
  }

  free(threads);
  free(thread_args);
}

bool contains_tuple(Partition *partition, Tuple *tuple) {
  for (size_t t = 0; t < partition->buffers->size; t++) {
    if (tuple->key == partition->buffers->tuples[t].key)
      return true;
  }

  return false;
}
