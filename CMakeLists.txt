cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
project(partitioning)

add_executable(generate_tuples generate_tuples.c tuple.c)
add_executable(concurrent main.c concurrent_partition.c tuple.c)
add_executable(parallelbuffers main.c parallelbuffers_partition.c tuple.c)

target_link_libraries(concurrent m)
target_link_libraries(parallelbuffers m)
