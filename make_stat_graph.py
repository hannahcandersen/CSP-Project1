from collections import defaultdict
import csv
from itertools import cycle
import matplotlib.pyplot as plt
import numpy as np
import sys

def main():
    if len(sys.argv) < 3:
        print("Wrong parameters")
        return

    filename = sys.argv[1]
    stat = sys.argv[2]
    title = sys.argv[3]
    save_fig = None
    if len(sys.argv) == 6 and sys.argv[4] == "--save":
        save_fig = sys.argv[5]


    marker_cycle = cycle(['o', 's', '^', 'D', 'v', 'p', '*', 'x', '+'])

    with open(filename, newline='') as csvfile:
        reader = csv.DictReader(csvfile)

        xs = defaultdict(set)
        ys = defaultdict(lambda: defaultdict(int))

        for row in reader:
            xs[row["threads"]].add(int(row["hash_bits"]))
            ys[row["threads"]][row["hash_bits"]] += int(row[stat])

        for key in xs.keys():
            x = np.array(list(xs[key]))
            y = 1 / 8 * np.array(list(ys[key].values())) # NOTE: Assumes 8 runs
            plt.plot(x, y, label=f"Threads: {key}", marker=next(marker_cycle))

        plt.xlabel("Hash Bits")
        plt.ylabel(stat)

        # TODO: Set y limit
        # plt.ylim(0, 200)
        plt.xticks([2 * x for x in range(0, 10)])

        plt.legend()
        plt.title(title)

        if save_fig is not None:
            plt.savefig(save_fig)
        else:
            plt.show()

if __name__ == "__main__":
    main()
