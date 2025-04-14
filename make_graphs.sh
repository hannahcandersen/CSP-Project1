algos=("concurrent" "parallel")
configs=("" "_affinity" "_affinity_bad" "_release" "_affinity_release")

for algo in "${algos[@]}"; do
    for config in "${configs[@]}"; do
        python make_graph.py "results/throughput/$algo$config.csv" "$algo$config" "16777216" "--save" "results/figures/throughput/$algo$config.png"
    done
done
