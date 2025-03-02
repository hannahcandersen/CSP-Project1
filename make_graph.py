from collections import defaultdict
import csv
import matplotlib.pyplot as plt
import numpy as np
import sys

def main():
    if len(sys.argv) < 3:
        print("Please provide name of data file and number of tuples")
        return

    filename = sys.argv[1]
    n_tuples = float(sys.argv[2])

    with open(filename, newline='') as csvfile:
        reader = csv.DictReader(csvfile)

        xs = defaultdict(list)
        ys = defaultdict(list)

        for row in reader:
            xs[row["threads"]].append(int(row["hash_bits"]))
            ys[row["threads"]].append(int(row["time (ms)"]))

        for key in xs.keys():
            x = np.array(xs[key])
            y = 1. / np.array(ys[key]) * n_tuples / 1000.
            plt.plot(x, y, label=f"Threads: {key}")

        plt.xlabel("Hash Bits")
        plt.ylabel("Thousands of Tuples per Second")
        plt.legend()
        plt.show()

if __name__ == "__main__":
    main()
