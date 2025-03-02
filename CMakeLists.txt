cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
project(partitioning)

# Generating data
add_executable(generate_tuples generate_tuples.c tuple.c)

# Running main algorithms
add_executable(concurrent main.c concurrent_partition.c tuple.c hash.c)
add_executable(parallelbuffers main.c parallelbuffers_partition.c tuple.c hash.c)

# Performance measuring
add_executable(concurrent_tuple_timing tuple_speed.c concurrent_partition.c tuple.c hash.c)
add_executable(parallelbuffers_tuple_timing tuple_speed.c parallelbuffers_partition.c tuple.c hash.c)

target_link_libraries(concurrent m)
target_link_libraries(parallelbuffers m)

target_link_libraries(concurrent_tuple_timing m)
target_link_libraries(parallelbuffers_tuple_timing m)
