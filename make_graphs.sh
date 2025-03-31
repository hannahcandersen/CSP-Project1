#!/usr/bin/sh

python make_graph.py results/parallel_results.csv "Parallel Buffers" 16777216 --save results/Parallel.png
python make_graph.py results/parallel_release_results.csv "Parallel Buffers with release flag (-O3)" 16777216 --save results/Parallel_release.png

python make_graph.py results/concurrent_results.csv "Concurrent" 16777216 --save results/Concurrent.png
python make_graph.py results/concurrent_release_results.csv "Concurrent with release flag (-O3)" 16777216 --save results/Concurrent_release.png

python make_graph.py results/parallel_results.csv "Parallel Buffers with core affinity" 16777216 --save results/Parallel_affinity.png
python make_graph.py results/parallel_release_results.csv "Parallel Buffers with release flag (-O3) & core affinity" 16777216 --save results/Parallel_affinity_release.png

python make_graph.py results/concurrent_results.csv "Concurrent with core affinity" 16777216 --save results/Concurrent_affinity.png
python make_graph.py results/concurrent_release_results.csv "Concurrent with release flag (-O3) & core affinity" 16777216 --save results/Concurrent_affinity_release.png

python make_graph.py results/concurrent_results_bad_affinity.csv "Concurrent with bad affinity" 16777216 --save results/Concurrent_bad_affinity.png
python make_graph.py results/concurrent_results_bad_affinity.csv "Parallel Buffers with bad affinity" 16777216 --save results/Parallel_bad_affinity.png
