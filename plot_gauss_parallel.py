import numpy as np
import matplotlib.pyplot as plt

# Load data
data = np.loadtxt("gauss_seidel_scaling.data", skiprows=1)

# Extract unique grid sizes and threads
grid_sizes = np.unique(data[:, 0])
threads = np.unique(data[:, 1])

plt.figure(figsize=(8,5))

# Plot speedup for each grid size
for N in grid_sizes:
    subset = data[data[:, 0] == N]
    plt.plot(subset[:, 1], subset[:, 3], marker='o', linestyle='-', label=f"Grid Size {int(N)}")

# Add ideal linear scaling line
ideal_speedup = np.array(threads)  # Ideal case where speedup = number of threads
plt.plot(threads, ideal_speedup, linestyle='--', color='black', label="Ideal Linear Scaling")

# Labels and title
plt.xlabel("Number of Threads")
plt.ylabel("Speedup")
plt.title("Gauss-Seidel Speedup vs. Number of Threads for Different Grid Sizes")
plt.xticks(threads.astype(int))  # Show thread count labels
plt.yticks(np.arange(1, max(threads)+1, step=1))
plt.legend()
plt.grid(True, which="both", linestyle="--", linewidth=0.5)

# Save and Show
plt.savefig("gauss_seidel_speedup.png")
plt.show()
