cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
project(partitioning)

# Generating data
add_executable(generate_tuples generate_tuples.c tuple.c)

# Running main algorithms
add_executable(concurrent main.c concurrent_partition.c tuple.c hash.c)
add_executable(parallelbuffers main.c parallelbuffers_partition.c tuple.c hash.c)

add_executable(concurrent_affinity main.c concurrent_partition.c tuple.c hash.c)
add_executable(parallelbuffers_affinity main.c parallelbuffers_partition.c tuple.c hash.c)
# Add definitions to compile programs with to set core affinity
target_compile_definitions(concurrent_affinity PRIVATE AFFINITY _GNU_SOURCE)
target_compile_definitions(parallelbuffers_affinity PRIVATE AFFINITY _GNU_SOURCE)

add_executable(concurrent_affinity_bad main.c concurrent_partition.c tuple.c hash.c)
add_executable(parallelbuffers_affinity_bad main.c parallelbuffers_partition.c tuple.c hash.c)

# Performance measuring
add_executable(concurrent_tuple_timing tuple_speed.c concurrent_partition.c tuple.c hash.c)
add_executable(parallelbuffers_tuple_timing tuple_speed.c parallelbuffers_partition.c tuple.c hash.c)

add_executable(concurrent_tuple_timing_affinity tuple_speed.c concurrent_partition.c tuple.c hash.c)
add_executable(parallelbuffers_tuple_timing_affinity tuple_speed.c parallelbuffers_partition.c tuple.c hash.c)

add_executable(concurrent_tuple_timing_affinity_bad tuple_speed.c concurrent_partition.c tuple.c hash.c)
add_executable(parallelbuffers_tuple_timing_affinity_bad tuple_speed.c parallelbuffers_partition.c tuple.c hash.c)

# Add definitions to compile programs with to set core affinity
target_compile_definitions(concurrent_tuple_timing_affinity PRIVATE AFFINITY _GNU_SOURCE)
target_compile_definitions(parallelbuffers_tuple_timing_affinity PRIVATE AFFINITY _GNU_SOURCE)

# Bad core affinity executables
target_compile_definitions(concurrent_affinity_bad PRIVATE AFFINITY BAD_AFFINITY _GNU_SOURCE)
target_compile_definitions(parallelbuffers_affinity_bad PRIVATE AFFINITY BAD_AFFINITY _GNU_SOURCE)
target_compile_definitions(concurrent_tuple_timing_affinity_bad PRIVATE AFFINITY BAD_AFFINITY _GNU_SOURCE)
target_compile_definitions(parallelbuffers_tuple_timing_affinity_bad PRIVATE AFFINITY BAD_AFFINITY _GNU_SOURCE)

target_link_libraries(parallelbuffers m)
target_link_libraries(parallelbuffers_affinity m)
target_link_libraries(parallelbuffers_affinity_bad m)
target_link_libraries(parallelbuffers_tuple_timing m)
target_link_libraries(parallelbuffers_tuple_timing_affinity m)
target_link_libraries(parallelbuffers_tuple_timing_affinity_bad m)
