#!/bin/bash

OUTPUT_FILE="jacobi_convergence.data"

# Define grid sizes and tolerances
GRID_SIZES=(50 100 200 300)
TOLERANCES=(1e-1 1e-3 1e-6)

# Write the header
echo "# Iteration Grid_Size Tolerance Diff" > $OUTPUT_FILE

for N in "${GRID_SIZES[@]}"; do
    for T in "${TOLERANCES[@]}"; do
        echo "Running Sequential Jacobi for Grid Size: $N with Tolerance: $T"

        # Run Jacobi with version 0 (sequential)
        output=$(./poisson_j $N 1000 $T 20 0)

        # Extract iteration number and diff values correctly
        echo "$output" | awk -v N="$N" -v T="$T" '/Iteration/ {print $2, N, T, $(NF)}' >> $OUTPUT_FILE

    done
done

echo "Data saved to $OUTPUT_FILE"
