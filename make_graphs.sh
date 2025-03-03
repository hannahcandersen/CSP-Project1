#!/usr/bin/sh

python make_graph.py results/parallel_results.csv "Parallel Buffers" 16777216 --save results/Parallel.png
python make_graph.py results/parallel_release_results.csv "Parallel Buffers (with release flag (-O3))" 16777216 --save results/Parallel_release.png

python make_graph.py results/concurrent_results.csv "Concurrent Buffers" 16777216 --save results/Concurrent.png
python make_graph.py results/concurrent_release_results.csv "Concurrent Buffers (with release flag (-O3))" 16777216 --save results/Concurrent_release.png
