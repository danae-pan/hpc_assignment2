#!/bin/bash
#BSUB -J jacobi_noopt_v2_benchmark
#BSUB -o jacobi_noopt_v2_benchmark_%j.out
#BSUB -q hpcintro
#BSUB -n 24
#BSUB -R "rusage[mem=10GB]"
#BSUB -W 00:40
# assures that your job has a whole CPU for itself (shared L3 cache)
#BSUB -R "span[hosts=1]"

# Use ⁠ nodestat -F hpcintro ⁠ to figure out what models are available
#BSUB -R "select[model == XeonE5_2650v4]"

# Define different grid sizes and thread counts
GRID_SIZES=(50 100 200 300 400)
THREADS=(1 2 4 8 16)

# Output file
OUTPUT_FILE="jacobi_noopt_results_v2.data"
echo "# Version N Threads Iterations Tolerance Final_Diff Time(s)" > $OUTPUT_FILE  # Add header

# for N in "${GRID_SIZES[@]}"; do
#     for T in "${THREADS[@]}"; do
#         export OMP_NUM_THREADS=$T  # Set number of OpenMP threads

#         echo "Running Grid Size: $N with $T threads"

#         # Run the Jacobi solver and append results to file
#         ./poisson_j $N 1000 1e-2 20 2  # Run parallel version (1)
#     done
# done

for T in "${THREADS[@]}"; do
    export OMP_NUM_THREADS=$T  # Set number of OpenMP threads

    echo "Running Grid Size: 300 with $T threads"

    # Run the Jacobi solver and append results to file
    ./poisson_j 300 1000 1e-2 20 2 # Run parallel version (1)
done

echo "Benchmark completed. Results saved to $OUTPUT_FILE"