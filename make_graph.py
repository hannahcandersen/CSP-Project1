from collections import defaultdict
import csv
from itertools import cycle
import matplotlib.pyplot as plt
import numpy as np
import sys

def main():
    if len(sys.argv) < 4:
        print("Wrong parameters")
        return

    filename = sys.argv[1]
    title = sys.argv[2]
    n_tuples = float(sys.argv[3])
    save_fig = None
    if len(sys.argv) == 6 and sys.argv[4] == "--save":
        save_fig = sys.argv[5]


    marker_cycle = cycle(['o', 's', '^', 'D', 'v', 'p', '*', 'x', '+'])

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
            plt.plot(x, y, label=f"Threads: {key}", marker=next(marker_cycle))

        plt.xlabel("Hash Bits")
        plt.ylabel("Millions of Tuples per Second")

        plt.ylim(0, 200)
        plt.xticks([2 * x for x in range(0, 10)])

        plt.legend()
        plt.title(f"{title} - 2^24 Tuples")

        if save_fig is not None:
            plt.savefig(save_fig)
        else:
            plt.show()

if __name__ == "__main__":
    main()
