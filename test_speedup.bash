#!/bin/bash

OUTPUT_FILE="speedup_threads_grids.data"

# Define different grid sizes and thread counts
GRID_SIZES=(50 100 200 300)
THREADS=(1 2 4 8 16)

# Write the header
echo "# Grid_Size Threads Parallel_Time Optimized_Parallel_Time Speedup" > $OUTPUT_FILE

for N in "${GRID_SIZES[@]}"; do
    for T in "${THREADS[@]}"; do
        export OMP_NUM_THREADS=$T  # Set number of OpenMP threads
        echo "Running Grid Size: $N with $T threads"

        # Run Parallel Jacobi and extract execution time (also calls optimized)
        output=$(./poisson_j $N 1000 1e-6 20 1)

        parallel_time=$(echo "$output" | awk '/Execution Time \(Parallel\)/ {print $(NF-1)}')
        optimized_time=$(echo "$output" | awk '/Execution Time \(Parallel Optimized\)/ {print $(NF-1)}')

        # Compute Speedup
        if [[ -n "$parallel_time" && -n "$optimized_time" ]]; then
            speedup=$(awk -v p="$parallel_time" -v o="$optimized_time" 'BEGIN {print p/o}')
        else
            speedup="N/A"
        fi

        # Append results to the data file
        echo "$N $T $parallel_time $optimized_time $speedup" >> $OUTPUT_FILE
    done
done

echo "Data saved to $OUTPUT_FILE"
