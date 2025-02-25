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
  Tuple *tuples;         
  size_t n_tuples;
  size_t hash_bits;      
} ThreadArgs;

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

    partitions[i].n_buffers = 1;
    partitions[i].buffers = buffer;
  }
}

void *partition_worker(void *args)
{
  ThreadArgs *t_args = args;
  size_t id = t_args->id;
  Partition *partitions = t_args->partitions;
  size_t n_partitions = t_args->n_partitions;
  Tuple *tuples = t_args->tuples;
  size_t n_tuples = t_args->n_tuples;
  size_t hash_bits = t_args->hash_bits;

  size_t *current_chunk = malloc(n_partitions * sizeof(size_t)); 
  size_t *first_chunk = malloc(n_partitions * sizeof(size_t)); 
  for (size_t i = 0; i < n_partitions; i++) {
    current_chunk[i] = NO_CHUNK;
    first_chunk[i] = NO_CHUNK;
  }

  /* WRITING PHASE */ 
  for (int i = 0; i < n_tuples; i++) {
    Tuple *tuple = &tuples[i];
    size_t partition = hash(tuple, hash_bits);
    Buffer *buffer = &partitions[partition].buffers[0];

    if (current_chunk[partition] == NO_CHUNK) {
      current_chunk[partition] = atomic_fetch_add(&buffer->h, 1);
      first_chunk[partition] = current_chunk[partition];
      //printf("Thread %zu has claimed a new chunk %zu for partition %zu\n", id, current_chunk[partition], partition);
    }

    if (buffer->d[id] >= buffer->c) {
      current_chunk[partition] = atomic_fetch_add(&buffer->h, 1);
      buffer->d[id] = 0;
      //printf("Thread %zu has claimed a new chunk %zu for partition %zu\n", id, current_chunk[partition], partition);
    }

    size_t offset = current_chunk[partition] * buffer->c + buffer->d[id];
    buffer->tuples[offset] = *tuple;
    buffer->d[id]++;
    //printf("Thread %zu has written tuple %zu to partition %zu at offset %zu\n", id, id * n_tuples + i, partition, offset);
  }

  /* TOP-UP PHASE */
  for (size_t p = 0; p < n_partitions; p++) {
    if (first_chunk[p] != current_chunk[p]) {
      //printf("Thread %zu is top-upping partition %zu\n", id, p);
      Buffer *buffer = &partitions[p].buffers[0];
      if (buffer->d[id] < buffer->c) { 
        size_t first_offset = first_chunk[p] * buffer->c;
        size_t last_offset = current_chunk[p] * buffer->c + buffer->d[id];
        size_t tuples_to_move = buffer->c - buffer->d[id];
        for (size_t i = 0; i < tuples_to_move; i++) {
          buffer->tuples[last_offset + i] = buffer->tuples[first_offset + i];
          buffer->tuples[first_offset + i] = (Tuple){0}; // Clear the old position
          //printf("Thread %zu has moved tuple %zu from chunk %zu / offset %zu to chunk %zu / offset %zu for partition %zu\n", id, id * n_tuples + i, first_chunk[p], first_offset + i, current_chunk[p], last_offset + i, p);
        }
      }
    }
  }

  free(current_chunk);
  free(first_chunk);
  return NULL;
}

void partition(Partition *partitions, size_t n_partitions, Tuple *tuples, 
               size_t n_tuples, size_t hash_bits, size_t n_threads) {
  pthread_t threads[n_threads];

  size_t tuples_per_thread = n_tuples / n_threads;

  ThreadArgs *thread_args = malloc(n_threads * sizeof(ThreadArgs));

  for (size_t t = 0; t < n_threads; t++) {
    thread_args[t].id = t;
    thread_args[t].partitions = partitions;
    thread_args[t].n_partitions = n_partitions;
    thread_args[t].tuples = &tuples[t * tuples_per_thread];
    thread_args[t].n_tuples = tuples_per_thread;
    thread_args[t].hash_bits = hash_bits;

    pthread_create(&threads[t], NULL, partition_worker, &thread_args[t]);
  }

  for (size_t t = 0; t < n_threads; t++) {
    pthread_join(threads[t], NULL);
  }

  free(thread_args);
}

bool contains_tuple(Partition *partition, Tuple *tuple) {
  for (size_t t = 0; t < partition->buffers->size; t++) {
    if (tuple->key == partition->buffers->tuples[t].key)
      return true;
  }

  return false;
}
