#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hash.c"
#include "partition.h"
#include "tuple.h"

int main(int argc, char **argv) {
  if (argc == 2 && (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0)) {
    printf("%s <filename> <#hash_bits> <#threads>\n", argv[0]);
    return 0;
  }
  else if (argc != 4) {
    printf("Wrong parameter count!\n");
    printf("Run with -h or --help to see how to run\n");
    return 1;
  }

  char *filename = argv[1];
  size_t hash_bits = atoi(argv[2]);
  size_t n_threads = atoi(argv[3]);

  size_t n_tuples;
  Tuple *tuples = read_tuples(filename, &n_tuples);

  size_t n_partitions = pow(2.0, (double)hash_bits);
  Partition *partitions = (Partition *)malloc(n_partitions * sizeof(Partition));

  make_buffers(partitions, n_partitions, n_tuples);

  partition(partitions, n_partitions, tuples, n_tuples, hash_bits, n_threads);

  for (size_t i = 0; i < n_tuples; i++) {
    printf("Key: %llu, Hash value: %zu\n", tuples[i].key, hash(&tuples[i], hash_bits));
  }

  return 0;
}
