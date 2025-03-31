./build/concurrent_tuple_timing data/tuples.txt results/results-concurrent.csv
./build/concurrent_tuple_timing_affinity data/tuples.txt results/results-concurrent_affinity.csv
./build/concurrent_tuple_timing_affinity_bad data/tuples.txt results/results-concurrent_affinity_bad.csv

./build/parallelbuffers_tuple_timing data/tuples.txt results/results-parallelbuffers.csv
./build/parallelbuffers_tuple_timing_affinity data/tuples.txt results/results-parallelbuffers_affinity.csv
./build/parallelbuffers_tuple_timing_affinity_bad data/tuples.txt results/results-parallelbuffers_affinity_bad.csv

./stat.sh build/concurrent data/tuples.csv results/stats-concurrent.txt
./stat.sh build/parallel data/tuples.csv results/stats-parallel.txt

./stat.sh build/concurrent_affinity data/tuples.csv results/stats-concurrent-affinity.txt
./stat.sh build/parallel_affinity data/tuples.csv results/stats-parallel-affinity.txt
