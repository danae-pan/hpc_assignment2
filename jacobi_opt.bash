#!/bin/bash
#BSUB -J jacobi_opt_benchmark
#BSUB -o jacobi_opt_benchmark_%j.out
#BSUB -q hpcintro
#BSUB -n 24
#BSUB -R "rusage[mem=10GB]"
#BSUB -W 00:30
# assures that your job has a whole CPU for itself (shared L3 cache)
#BSUB -R "span[hosts=1]"

# Use ⁠ nodestat -F hpcintro ⁠ to figure out what models are available
#BSUB -R "select[model == XeonE5_2650v4]"

# Define different grid sizes and thread counts
GRID_SIZES=(50 100 200 300 400)
THREADS=(1 2 4 8 16)
VERSIONS=(1 2)
# Output file
OUTPUT_FILE="jacobi_opt_results.data"
echo "# Version N Threads Iterations Tolerance Final_Diff Time(s)" > $OUTPUT_FILE  # Add header

# UNCOMMENT IF YOU WANT TO RUN FOR DIFFERENT GRID SIZES
# for version in "${VERSIONS[@]}"; do
#     for N in "${GRID_SIZES[@]}"; do
#         for T in "${THREADS[@]}"; do
#             export OMP_NUM_THREADS=$T  # Set number of OpenMP threads

#             echo "Running Grid Size: $N with $T threads"

#             # Run the Jacobi solver and append results to file
#             ./poisson_j_opt 300 1000 1e-2 20 $version # Run parallel version (1)
#         done
#     done
# done

# COMMENT IF YOU WANT TO RUN FOR DIFFERENT GRID SIZES:
for version in "${VERSIONS[@]}"; do
    for T in "${THREADS[@]}"; do
        export OMP_NUM_THREADS=$T  # Set number of OpenMP threads

        echo "Running Grid Size: $N with $T threads"

        # Run the Jacobi solver and append results to file
        ./poisson_j_opt 300 1000 1e-2 20 $version # Run parallel version (1)
    done
done

echo "Benchmark completed. Results saved to $OUTPUT_FILE"