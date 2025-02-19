#include <stdio.h>
#include <stdlib.h>
#include "tuple.h"

int main(int argc, char **argv) {
  if (argc < 3) {
    printf("Must take 2 parameters (#tuples and filename to save to)\n");
    return 1;
  }

  unsigned int n_tuples = atoi(argv[1]);
  char *filename = argv[2];

  printf("Generate %d tuples\n", n_tuples);
  printf("Write tuples to file: %s\n", filename);

  Tuple *tuples = malloc(n_tuples * sizeof(Tuple));

  printf("Memomry allocated\n");

  for (int i = 0; i < n_tuples; i++) {
    tuples[i] = rand_tuple();
  }

  printf("Tuples generated\n");

  for (int i = 0; i < n_tuples; i++) {
    printf("Key: %llu, Value: %llu\n", tuples[i].key, tuples[i].value);
  }

  FILE *file = fopen(filename, "w");
  if (file == NULL) {
    printf("Failed to open file\n");
    return 1;
  }

  printf("File opened\n");

  fprintf(file, "%u\n", n_tuples);

  for (int i = 0; i < n_tuples; i++) {
    fprintf(file, "%llu,%llu\n", tuples[i].key, tuples[i].value);
  }

  printf("Tuples written to file\n");

  fclose(file);
  free(tuples);

  TupleArray *ta = read_tuples(filename);

  for (size_t i = 0; i < ta->size; i++) {
    printf("Key: %llu, Value: %llu\n", ta->tuples[i].key, ta->tuples[i].value);
  }

  free(ta->tuples);
  free(ta);

  return 0;
}
