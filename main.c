#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hash.h"
#include "partition.h"
#include "tuple.h"

int main(int argc, char **argv) {
  bool verify_result = false;
  if (argc == 2 && (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0)) {
    printf("%s <filename> <#hash_bits> <#threads> [--verify]\n", argv[0]);
    return 0;
  }
  else if (argc < 4) {
    printf("Wrong parameter count!\n");
    printf("Run with -h or --help to see how to run\n");
    return 1;
  }
  if (argc == 5 && strcmp(argv[4], "--verify") == 0) {
    verify_result = true;
  }

  char *filename = argv[1];
  size_t hash_bits = atoi(argv[2]);
  size_t n_threads = atoi(argv[3]);

  size_t n_tuples;
  Tuple *tuples = read_tuples(filename, &n_tuples);

  size_t n_partitions = pow(2.0, (double)hash_bits);
  Partition *partitions = (Partition *)malloc(n_partitions * sizeof(Partition));

  make_buffers(partitions, n_partitions, n_tuples, n_threads);

  partition(partitions, n_partitions, tuples, n_tuples, hash_bits, n_threads);

  printf("Done partitioning\n");

  if (verify_result) {
    printf("Verifying results...\n");
    for (size_t i = 0; i < n_tuples; i++) {
      if (i % 1000 == 0) {
        printf("%f%%\r", (double)i / (double)n_tuples * 100.);
      }
      size_t partition_idx = hash(&tuples[i], hash_bits);
      if (!contains_tuple(&partitions[partition_idx], &tuples[i])) {
        printf("Tuple with index %zu should be in partition %zu, but isn't!\n", i, partition_idx);
      }
    }
    printf("Results verified\n");
  }

  return 0;
}
