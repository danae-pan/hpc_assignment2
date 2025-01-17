import numpy as np
import matplotlib.pyplot as plt

# Load data
data = np.loadtxt("speedup_threads_grids.data", skiprows=1, dtype=str)

# Extract unique grid sizes, threads, and placements
grid_sizes = np.unique(data[:, 0].astype(int))
threads = np.unique(data[:, 1].astype(int))
placements = np.unique(data[:, 2])

plt.figure(figsize=(8,5))

# Plot speedup for each grid size and placement
for N in grid_sizes:
    for placement in placements:
        subset = data[(data[:, 0].astype(int) == N) & (data[:, 2] == placement)]
        if subset.size > 0:
            plt.plot(subset[:, 1].astype(int), subset[:, 5].astype(float), marker='o', linestyle='-', label=f"Grid Size {int(N)}, {placement}")

# Labels and title
plt.xlabel("Number of Threads")
plt.ylabel("Speedup (Parallel / Optimized Parallel)")
plt.title("Speedup vs. Number of Threads for Different Grid Sizes and Placements")
plt.xticks(threads.astype(int))  # Show thread count labels
plt.legend()
plt.grid(True, which="both", linestyle="--", linewidth=0.5)

# Save and Show
plt.savefig("speedup_threads_grids_placements.png")
plt.show()
