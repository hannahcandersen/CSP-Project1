#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "hash.h"
#include "partition.h"
#include "tuple.h"

#define N_TRIALS 8l

int main(int argc, char **argv) {
  if (argc == 2 &&
      (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0)) {
    printf("%s <tuple_filename> <result_filename>\n", argv[0]);
    return 0;
  }
  if (argc < 3) {
    printf("Something with parameters went wrong!\n");
    printf("Run with -h or --help to see how to run\n");
    return 1;
  }

  char *tuple_filename = argv[1];
  char *results_filename = argv[2];

  FILE *results_file = fopen(results_filename, "w");
  if (!results_filename) {
    perror("Couldn't open results file!\n");
    return 1;
  }
  // Print header of csv
  fprintf(results_file, "hash_bits,threads,time (ms)\n");

  size_t n_tuples;
  Tuple *tuples = read_tuples(tuple_filename, &n_tuples);

  size_t different_thread_counts = 6;
  size_t *n_threads = malloc(different_thread_counts * sizeof(size_t));
  for (size_t i = 0; i < different_thread_counts; i++) {
    n_threads[i] = 1 << i;
  }

  size_t different_hash_bits = 18;
  size_t *hash_bits = malloc(different_hash_bits * sizeof(size_t));
  for (size_t i = 0; i < different_hash_bits; i++) {
    hash_bits[i] = i + 1;
  }

  struct timespec start, finish;

  for (size_t b = 0; b < different_hash_bits; b++) {
    size_t n_partitions = 1 << hash_bits[b];
    Partition *partitions =
        (Partition *)malloc(n_partitions * sizeof(Partition));

    for (size_t t = 0; t < different_thread_counts; t++) {
      long elapsed_time_ms = 0l;

      for (long i = 0; i < N_TRIALS; i++) {
        // Allocate buffers
        make_buffers(partitions, n_partitions, n_tuples, n_threads[t]);

        // Time partition
        clock_gettime(CLOCK_MONOTONIC_RAW, &start);
        // TODO: Average of 8 trials
        partition(partitions, n_partitions, tuples, n_tuples, hash_bits[b],
                  n_threads[t]);
        clock_gettime(CLOCK_MONOTONIC_RAW, &finish);

        elapsed_time_ms += (finish.tv_sec - start.tv_sec) * 1000l +
                           (finish.tv_nsec - start.tv_nsec) / 1000000l;

        // Free buffers because they depend on threads in some
        // implementations
        free_buffers(partitions, n_partitions, n_tuples, n_threads[t]);
      }
      printf("%zu, %zu: %lu ms\n", hash_bits[b], n_threads[t], elapsed_time_ms / N_TRIALS);
      fprintf(results_file, "%zu,%zu,%ld\n", hash_bits[b], n_threads[t],
              elapsed_time_ms);
    }

    // Partitions depend on hash bits
    free(partitions);
  }

  free(tuples);

  fclose(results_file);

  return 0;
}
