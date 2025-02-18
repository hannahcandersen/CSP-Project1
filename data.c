#include <stdio.h>
#include <stdlib.h>

struct Tuple {
  unsigned long long int key;
  unsigned long long int value;
};

unsigned long long int rand_long() {
  return rand() | (long long int)rand() >> 32;
}

int main(int argc, char** argv) {
  if (argc < 3) {
    printf("Must take 3 parameters\n");
    return 1;
  }

  unsigned int n_tuples = atoi(argv[1]);
  char *filename = argv[2];

  printf("Generate %d tuples\n", n_tuples);
  printf("Write tuples to file: %s\n", filename);

  struct Tuple *tuples = malloc(n_tuples * sizeof(struct Tuple));

  return 0;
}
