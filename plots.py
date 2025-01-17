import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

# Load the data from the file
file_path = "jacobi_opt_results.data"  # Change to "jacobi_noopt_results.data" to plot unoptimized results

# Read data while skipping comment lines
data = pd.read_csv(file_path, delim_whitespace=True, comment="#", 
                   names=["Version", "N", "Threads", "Iterations", "Tolerance", "Final_Diff", "Time"])

# Compute MLUP/s: Millions of Lattice Updates per Second
data["MLUPs"] = (data["N"]**3 * data["Iterations"]) / (data["Time"] * 1e6)

# Group data by grid size (N)
grid_sizes = sorted(data["N"].unique())

# 1️⃣ **Execution Time vs. Threads**
plt.figure(figsize=(8, 6))
for N in grid_sizes:
    subset = data[data["N"] == N]
    plt.plot(subset["Threads"], subset["Time"], marker="o", label=f"N={N}")
plt.xlabel("Number of Threads")
plt.ylabel("Execution Time (s)")
plt.title("Execution Time vs. Number of Threads")
plt.xscale("log", base=2)
plt.legend()
plt.grid()
plt.savefig("execution_time_vs_threads.png")
plt.show()

# 2️⃣ **Speedup vs. Threads**
plt.figure(figsize=(8, 6))
for N in grid_sizes:
    subset = data[data["N"] == N]
    baseline_time = subset[subset["Threads"] == 1]["Time"].values[0]
    plt.plot(subset["Threads"], baseline_time / subset["Time"], marker="o", label=f"N={N}")
plt.xlabel("Number of Threads")
plt.ylabel("Speedup")
plt.title("Speedup vs. Number of Threads")
plt.xscale("log", base=2)
plt.yscale("log", base=2)
plt.legend()
plt.grid()
plt.savefig("speedup_vs_threads.png")
plt.show()

# 3️⃣ **MLUP/s vs. Threads**
plt.figure(figsize=(8, 6))
for N in grid_sizes:
    subset = data[data["N"] == N]
    plt.plot(subset["Threads"], subset["MLUPs"], marker="o", label=f"N={N}")
plt.xlabel("Number of Threads")
plt.ylabel("MLUP/s (Millions of Lattice Updates per Second)")
plt.title("MLUP/s vs. Number of Threads")
plt.xscale("log", base=2)
plt.legend()
plt.grid()
plt.savefig("mlups_vs_threads.png")
plt.show()

# 4️⃣ **Execution Time vs. Grid Size**
plt.figure(figsize=(8, 6))
for T in sorted(data["Threads"].unique()):
    subset = data[data["Threads"] == T]
    plt.plot(subset["N"], subset["Time"], marker="o", label=f"Threads={T}")
plt.xlabel("Grid Size (N)")
plt.ylabel("Execution Time (s)")
plt.title("Execution Time vs. Grid Size")
plt.legend()
plt.grid()
plt.savefig("execution_time_vs_N.png")
plt.show()

# 5️⃣ **Iterations to Convergence vs. Grid Size**
plt.figure(figsize=(8, 6))
for T in sorted(data["Threads"].unique()):
    subset = data[data["Threads"] == T]
    plt.plot(subset["N"], subset["Iterations"], marker="o", label=f"Threads={T}")
plt.xlabel("Grid Size (N)")
plt.ylabel("Iterations to Convergence")
plt.title("Iterations vs. Grid Size")
plt.legend()
plt.grid()
plt.savefig("iterations_vs_N.png")
plt.show()