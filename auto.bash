#!/bin/bash

# Define the versions to test (0: Sequential, 1: Parallel, 2: Optimized)
VERSIONS=(0 1 2)

# Define the range of threads to test for parallel versions
THREADS=(1 2 4 8 16)

# Define program parameters
GRID_SIZE=100
MAX_ITER=50
THRESHOLD=1e-6
START_TEMP=20

# Output file for results
OUTPUT_FILE="jacobi_detailed_results.txt"

# Remove old results file
rm -f $OUTPUT_FILE

# Write header to output file
echo "Version, Threads, Iterations, Final max_diff, Execution Time (s)" >> $OUTPUT_FILE

# Loop through each version
for version in "${VERSIONS[@]}"; do
    echo "Testing version $version..."
    if [ "$version" -eq 0 ]; then
        # Sequential version: No threading
        ./poisson_j $GRID_SIZE $MAX_ITER $THRESHOLD $START_TEMP $version | grep "Version:" | awk -F', ' '{print $1", " $2", " $3", " $4", " $5}' >> $OUTPUT_FILE
    else
        # Parallel versions
        for t in "${THREADS[@]}"; do
            echo "Running version $version with $t threads..."
            export OMP_NUM_THREADS=$t
            ./poisson_j $GRID_SIZE $MAX_ITER $THRESHOLD $START_TEMP $version | grep "Version:" | awk -F', ' '{print $1", " $2", " $3", " $4", " $5}' >> $OUTPUT_FILE
        done
    fi
done

echo "Testing complete. Results saved to $OUTPUT_FILE."
