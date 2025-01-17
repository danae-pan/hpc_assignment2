#!/bin/bash

OUTPUT_FILE="speedup_threads_grids.data"

GRID_SIZES=(50 100 200 300)
THREADS=(1 2 4 8 16)
PLACEMENTS=("spread" "close" "master")  # Available placement strategies

echo "# Grid_Size Threads Placement Parallel_Time Optimized_Parallel_Time Speedup" > $OUTPUT_FILE

for N in "${GRID_SIZES[@]}"; do
    for T in "${THREADS[@]}"; do
        for P in "${PLACEMENTS[@]}"; do
            export OMP_NUM_THREADS=$T
            export OMP_PROC_BIND=$P  # Apply thread placement

            echo "Running Grid Size: $N with $T threads (Placement: $P)"

            output=$(./poisson_j $N 1000 1e-6 20 1)

            parallel_time=$(echo "$output" | awk '/Execution Time \(Parallel\)/ {print $(NF-1)}')
            optimized_time=$(echo "$output" | awk '/Execution Time \(Parallel Optimized\)/ {print $(NF-1)}')

            if [[ -n "$parallel_time" && -n "$optimized_time" ]]; then
                speedup=$(awk -v p="$parallel_time" -v o="$optimized_time" 'BEGIN {print p/o}')
            else
                speedup="N/A"
            fi

            echo "$N $T $P $parallel_time $optimized_time $speedup" >> $OUTPUT_FILE
        done
    done
done

echo "Data saved to $OUTPUT_FILE"
