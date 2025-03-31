#include <stdio.h>

int main(int argc, char **argv) {
  if (argc < 3) {
    printf("./%s <stats-filename> <csv-filename>\n", argv[0]);
    return 1;
  }

  FILE *stats_file = fopen(argv[1], "r");

  if (stats_file == NULL) {
    printf("Failed to open stats file\n");
    return 1;
  }

  FILE *csv_file = fopen(argv[2], "w");

  if (csv_file == NULL) {
    printf("Failed to open csv file\n");
    return 1;
  }

  // Header of csv file
  fprintf(csv_file, "threads,hash_bits,run,instructions,cycles,cache_misses,"
                    "dTLB_load_misses,dTLB_store_misses\n");

  int threads, hash_bits, run;
  unsigned long long int instructions, cycles, cache_misses, dTLB_load_misses,
      dTLB_store_misses;
  while (fscanf(stats_file, "threads:%d,hash_bits:%d,run:%d", &threads,
                &hash_bits, &run) == 3) {
    if (!fscanf(stats_file, "%llu----instructions", &instructions)) {
      printf("Failed to read instructions for threads:%d,hash_bits:%d,run:%d\n",
             threads, hash_bits, run);
      instructions = 0;
    }
    while (fgetc(stats_file) != '\n' && !feof(stats_file));
    printf("Instructions: %llu\n", instructions);
    if (!fscanf(stats_file, "%llu----cycles", &cycles)) {
      printf("Failed to read cycles for threads:%d,hash_bits:%d,run:%d\n",
             threads, hash_bits, run);
      cycles = 0;
    }
    while (fgetc(stats_file) != '\n' && !feof(stats_file));
    if (!fscanf(stats_file, "%llu----dTLB-load-misses", &dTLB_load_misses)) {
      printf("Failed to read dTLB-load-misses for "
             "threads:%d,hash_bits:%d,run:%d\n",
             threads, hash_bits, run);
      dTLB_load_misses = 0;
    }
    while (fgetc(stats_file) != '\n' && !feof(stats_file));
    if (!fscanf(stats_file, "%llu----dTLB-store-misses", &dTLB_store_misses)) {
      printf("Failed to read dTLB-store-misses for "
             "threads:%d,hash_bits:%d,run:%d\n",
             threads, hash_bits, run);
      dTLB_store_misses = 0;
    }
    while (fgetc(stats_file) != '\n' && !feof(stats_file));
    if (!fscanf(stats_file, "%llu----cache-misses", &cache_misses)) {
      printf("Failed to read cache-misses for "
             "threads:%d,hash_bits:%d,run:%d\n",
             threads, hash_bits, run);
      cache_misses = 0;
    }
    while (fgetc(stats_file) != '\n' && !feof(stats_file));

    fprintf(csv_file, "%d,%d,%d,%llu,%llu,%llu,%llu,%llu\n", threads, hash_bits,
            run, instructions, cycles, cache_misses, dTLB_load_misses,
            dTLB_store_misses);
  }

  fclose(stats_file);
  fclose(csv_file);
}
