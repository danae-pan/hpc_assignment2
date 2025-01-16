import numpy as np
import matplotlib.pyplot as plt

# Load data (skip header)
data = np.loadtxt("jacobi_convergence.data", skiprows=1)

# Extract unique tolerances and grid sizes
tolerances = np.unique(data[:, 2])
grid_sizes = np.unique(data[:, 1])

# Define a vibrant but soft color palette
colors = ["#E63946", "#457B9D", "#F4A261", "#2A9D8F"]  # Pink, Blue, Yellow, Green

# Create a separate plot for each tolerance
for tol in tolerances:
    plt.figure(figsize=(8, 5))

    for i, N in enumerate(grid_sizes):
        subset = data[(data[:, 1] == N) & (data[:, 2] == tol)]
        if len(subset) > 0:
            # Plot with smooth lines and vibrant colors
            plt.plot(subset[:, 0], subset[:, 3], 
                     linestyle='-', linewidth=2, color=colors[i % len(colors)], 
                     label=f"Grid Size {int(N)}")

    # Labels and title
    plt.xlabel("Iteration Number", fontsize=12)
    plt.ylabel("Mean Squared Difference (Diff)", fontsize=12)
    plt.title(f"Convergence of Jacobi Method (Tolerance {tol:.1e})", fontsize=14)

    # Log scale for better visualization
    plt.yscale("log")
    
    # Adjust legend placement
    plt.legend(loc="upper right", fontsize=10, frameon=True)

    # Grid styling
    plt.grid(True, which="both", linestyle="--", linewidth=0.5, alpha=0.7)

    # Save plot for each tolerance
    plt.savefig(f"jacobi_convergence_tol_{tol:.1e}.png", dpi=300)

    # Show the plot
    plt.show()
