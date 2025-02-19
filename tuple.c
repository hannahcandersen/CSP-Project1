#include <stdio.h>
#include <stdlib.h>
#include <string.h>

# ifndef TUPLES
# define TUPLES
typedef struct Tuple {
  unsigned long long int key;
  unsigned long long int value;
} Tuple;

typedef struct TupleArray {
  Tuple *tuples;
  size_t size;
} TupleArray;

unsigned long long int rand_long() {
  return rand() | (unsigned long long int)rand() << 32;
}

Tuple rand_tuple() {
  Tuple t = {rand_long(), rand_long()};
  return t;
}

TupleArray *read_tuples(char *filename) {
  FILE *file = fopen(filename, "r");

  if (file == NULL) {
    printf("Couldn't open file\n");
    return NULL;
  }

  TupleArray *ta = malloc(sizeof(TupleArray));

  size_t size;

  if (fscanf(file, "%zu\n", &size) != 1) {
    printf("Couldn't parse size from file\n");
    return NULL;
  }

  ta->size = size;

  ta->tuples = malloc(size * sizeof(Tuple));

  unsigned long long int key;
  unsigned long long int value;
  for (size_t i = 0; i < size; i++) {
    if (fscanf(file, "%llu,%llu\n", &key, &value) != 2) {
      printf("Couldn't parse tuple #%zu from file\n", i);
      return NULL;
    }
    Tuple t = {key, value};
    ta->tuples[i] = t;
  }

  return ta;
}
# endif

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
