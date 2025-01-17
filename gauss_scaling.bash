#!/bin/bash

#BSUB -J gauss_scaling
#BSUB -o gauss_scaling_%j.out
#BSUB -q hpcintro
#BSUB -n 24
#BSUB -R "rusage[mem=10GB]"
#BSUB -W 00:30
# assures that your job has a whole CPU for itself (shared L3 cache)
#BSUB -R "span[hosts=1]"

# Use ⁠ nodestat -F hpcintro ⁠ to figure out what models are available
#BSUB -R "select[model == XeonE5_2650v4]"

OUTPUT_FILE="gauss_seidel_scaling.data"

# Define grid sizes and thread counts to test
GRID_SIZES=(100 200 300 400)
THREADS=(1 2 4 8 16)

# Write the header
echo "# Grid_Size Threads Parallel_Time Speedup" > $OUTPUT_FILE

for N in "${GRID_SIZES[@]}"; do
    for T in "${THREADS[@]}"; do
        export OMP_NUM_THREADS=$T
        echo "Running Parallel Gauss-Seidel for Grid Size: $N with $T threads"

        # Run Parallel Gauss-Seidel and extract execution time
        par_time=$(./poisson_gs $N 1000 1e-6 20 4 | awk '/Execution Time/ {print $(NF-1)}')

        # Compute Speedup (Relative to Single Thread Performance)
        if [[ -n "$par_time" ]]; then
            if [[ $T -eq 1 ]]; then
                base_time=$par_time  # Store single-thread time as baseline
            fi
            speedup=$(awk -v base="$base_time" -v p="$par_time" 'BEGIN {print base/p}')
        else
            speedup="N/A"
        fi

        # Append results to the data file
        echo "$N $T $par_time $speedup" >> $OUTPUT_FILE
    done
done

echo "Data saved to $OUTPUT_FILE"
