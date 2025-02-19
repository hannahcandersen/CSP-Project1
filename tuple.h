#include <stdio.h>
#include <stdlib.h>

# ifndef TUPLES
# define TUPLES
typedef struct Tuple {
  unsigned long long int key;
  unsigned long long int value;
} Tuple;

inline unsigned long long int rand_long() {
  return rand() | (unsigned long long int)rand() << 32;
}

inline Tuple rand_tuple() {
  Tuple t = {rand_long(), rand_long()};
  return t;
}

inline Tuple *read_tuples(char *filename, size_t *size) {
  FILE *file = fopen(filename, "r");

  if (file == NULL) {
    printf("Couldn't open file\n");
    return NULL;
  }

  if (fscanf(file, "%zu\n", size) != 1) {
    printf("Couldn't parse size from file\n");
    return NULL;
  }

  Tuple *tuples = (Tuple *)malloc(*size * sizeof(Tuple));

  unsigned long long int key;
  unsigned long long int value;
  for (size_t i = 0; i < *size; i++) {
    if (fscanf(file, "%llu,%llu\n", &key, &value) != 2) {
      printf("Couldn't parse tuple #%zu from file\n", i);
      return NULL;
    }
    tuples[i] = {key, value};
  }

  return tuples;
}
# endif
