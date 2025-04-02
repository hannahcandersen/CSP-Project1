#!/bin/bash

if [[ $1 == "-h" || $1 == "--help" ]] then
    echo "$0 <executable> <data-filename> <stats-filename>"
    exit 1
fi

for hash_bits in {1..18} ; do
    threads=1
    while [[ $threads -le 32 ]] ; do
        echo "Run with $threads threads and $hash_bits hash bits"
        for n_run in {1..8} ; do
            echo "threads:$threads,hash_bits:$hash_bits,run:$n_run" >> $3
            perf stat --event instructions,cycles,dTLB-load-misses,dTLB-store-misses,cache-misses -x -- $1 $2 $hash_bits $threads 2>> $3
        done
        (( threads *= 2 ))
    done
done
