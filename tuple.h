#include <stdio.h>
#include <stdlib.h>

#ifndef TUPLES
#define TUPLES

typedef struct {
  unsigned long long int key;
  unsigned long long int value;
} Tuple;

unsigned long long int rand_long();

Tuple rand_tuple();

Tuple *read_tuples(char *filename, size_t *size);

#endif
