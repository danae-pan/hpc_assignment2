import numpy as np
import matplotlib.pyplot as plt

# Load data
data = np.loadtxt("speedup_threads_grids_amdahl.data", skiprows=1, dtype=str)

# Extract unique values for grids, threads, placements, and versions
grid_sizes = np.unique(data[:, 0].astype(int))
threads = np.array([2, 4, 8, 16])  # Explicitly define thread counts for consistency
placements = np.unique(data[:, 2])
versions = np.unique(data[:, 3].astype(int))

# Function to safely convert values, replacing "N/A" with NaN
def safe_float_conversion(arr):
    return np.array([float(x) if x.replace('.', '', 1).isdigit() else np.nan for x in arr])

# Convert execution time and speedup to numerical values (handle "N/A")
execution_time = safe_float_conversion(data[:, 4])
speedup_values = safe_float_conversion(data[:, 5])

# Create two subplots: Parallel vs Optimized
fig, axes = plt.subplots(1, 2, figsize=(14, 5), sharey=True)

# --- Plot for Parallel Jacobi ---
for P in placements:
    for N in grid_sizes:
        subset = data[(data[:, 0].astype(int) == N) & (data[:, 2] == P) & (data[:, 3].astype(int) == 1)]
        thread_values = subset[:, 1].astype(int)
        speedup = safe_float_conversion(subset[:, 5])
        axes[0].plot(thread_values, speedup, marker='o', linestyle='-', label=f"Grid {N}, {P}")

axes[0].set_xlabel("Number of Threads")
axes[0].set_ylabel("Speedup")
axes[0].set_title("Speedup for Parallel Jacobi")
axes[0].set_xscale("log")
axes[0].set_xticks(threads)  # Ensure correct x-axis ticks
axes[0].set_xticklabels(threads)  # Display thread counts as labels
axes[0].legend()
axes[0].grid(True)

# --- Plot for Optimized Parallel Jacobi ---
for P in placements:
    for N in grid_sizes:
        subset = data[(data[:, 0].astype(int) == N) & (data[:, 2] == P) & (data[:, 3].astype(int) == 2)]
        thread_values = subset[:, 1].astype(int)
        speedup = safe_float_conversion(subset[:, 5])
        axes[1].plot(thread_values, speedup, marker='s', linestyle='--', label=f"Grid {N}, {P}")

axes[1].set_xlabel("Number of Threads")
axes[1].set_title("Speedup for Optimized Parallel Jacobi")
axes[1].set_xscale("log")
axes[1].set_xticks(threads)
axes[1].set_xticklabels(threads)
axes[1].legend()
axes[1].grid(True)

# Save and Show
plt.tight_layout()
plt.savefig("speedup_parallel_vs_optimized.png")
plt.show()

# --- Combined Plot for All Placements ---
fig, ax = plt.subplots(figsize=(10, 6))

for P in placements:
    for N in grid_sizes:
        subset_parallel = data[(data[:, 0].astype(int) == N) & (data[:, 2] == P) & (data[:, 3].astype(int) == 1)]
        subset_optimized = data[(data[:, 0].astype(int) == N) & (data[:, 2] == P) & (data[:, 3].astype(int) == 2)]
        
        thread_values_parallel = subset_parallel[:, 1].astype(int)
        speedup_parallel = safe_float_conversion(subset_parallel[:, 5])

        thread_values_optimized = subset_optimized[:, 1].astype(int)
        speedup_optimized = safe_float_conversion(subset_optimized[:, 5])

        ax.plot(thread_values_parallel, speedup_parallel, marker='o', linestyle='-', label=f"Grid {N}, {P}, Baseline")
        ax.plot(thread_values_optimized, speedup_optimized, marker='s', linestyle='--', label=f"Grid {N}, {P}, Improved")

ax.set_xlabel("Number of Threads")
ax.set_ylabel("Speedup")
ax.set_title("Speedup Comparison: Baseline vs Improved")
ax.set_xscale("log")
ax.set_xticks(threads)
ax.set_xticklabels(threads)
ax.legend()
ax.grid(True)

# Save and Show
plt.tight_layout()
plt.savefig("speedup_comparison.png")
plt.show()
