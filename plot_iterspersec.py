import matplotlib.pyplot as plt
import numpy as np

# Function to read data from file
def read_data(filename):
    grid_sizes = []
    iterations_per_sec = []
    total_iterations = []

    with open(filename, 'r') as file:
        for line in file:
            if line.startswith("#"):  # Skip header
                continue
            parts = line.split()
            grid_sizes.append(int(parts[1]))  # Grid_Size (N)
            iterations_per_sec.append(float(parts[3]))  # Iterations_per_sec
            total_iterations.append(int(parts[0]))  # Total Iterations
            
    return np.array(grid_sizes), np.array(iterations_per_sec), np.array(total_iterations)

# Read data for both methods
jacobi_N, jacobi_iters_per_sec, jacobi_total_iters = read_data("jacobi_iterspersec.data")
gauss_N, gauss_iters_per_sec, gauss_total_iters = read_data("gauss_iterspersec.data")

# Create figure with two subplots
fig, axes = plt.subplots(2, 1, figsize=(8, 10))

# Plot 1: Iterations per Second vs Grid Size
axes[0].plot(jacobi_N, jacobi_iters_per_sec, 'o-', label="Jacobi Method", color='blue')
axes[0].plot(gauss_N, gauss_iters_per_sec, 's-', label="Gauss-Seidel Method", color='red')
# axes[0].set_xlabel("Grid Size (N)")
axes[0].set_ylabel("Iterations per Second")
axes[0].set_title("Iterations per Second vs Grid Size until Convergence")
axes[0].legend()
axes[0].grid(True)
axes[0].set_yscale('log')  # Optional: Log scale for better visualization

# Plot 2: Total Iterations vs Grid Size
axes[1].plot(jacobi_N, jacobi_total_iters, 'o-', label="Jacobi Method", color='blue')
axes[1].plot(gauss_N, gauss_total_iters, 's-', label="Gauss-Seidel Method", color='red')
axes[1].set_xlabel("Grid Size (N)")
axes[1].set_ylabel("Total Iterations")
axes[1].set_title("Total Iterations vs Grid Size until Convergence")
axes[1].legend()
axes[1].grid(True)

# Show the plots
# plt.tight_layout()
plt.show()