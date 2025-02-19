#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "partition.h"
#include "tuple.h"

int main(int argc, char **argv) {
  if (argc != 4) {
    printf("Must supply filename to read from\n");
    return 1;
  }

  char *filename = argv[1];
  size_t hash_bits = atoi(argv[2]);
  size_t n_threads = atoi(argv[3]);

  size_t n_tuples;
  Tuple *tuples = read_tuples(filename, &n_tuples);

  size_t n_partitions = 4;
  Partition *partitions = (Partition *)malloc(n_partitions * sizeof(Partition));

  make_buffers(partitions, n_partitions, n_tuples);

  // TODO: Time
  partition(partitions, n_partitions, tuples, n_tuples, hash_bits, n_threads);

  return 0;
}
