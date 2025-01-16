import numpy as np
import matplotlib.pyplot as plt

# Load data
data = np.loadtxt("speedup_threads_grids.data", skiprows=1)

# Extract unique grid sizes and threads
grid_sizes = np.unique(data[:, 0])
threads = np.unique(data[:, 1])

plt.figure(figsize=(8,5))

# Plot speedup for each grid size
for N in grid_sizes:
    subset = data[data[:, 0] == N]
    plt.plot(subset[:, 1], subset[:, 4], marker='o', linestyle='-', label=f"Grid Size {int(N)}")


# Labels and title
plt.xlabel("Number of Threads")
plt.ylabel("Speedup (Parallel / Optimized Parallel)")
plt.title("Speedup vs. Number of Threads for Different Grid Sizes")
plt.xticks(threads.astype(int))  # Show thread count labels
plt.legend()
plt.grid(True, which="both", linestyle="--", linewidth=0.5)

# Save and Show
plt.savefig("speedup_threads_grids.png")
plt.show()
