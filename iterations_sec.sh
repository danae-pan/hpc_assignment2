#!/bin/bash

#BSUB -J gauss_iterspersec
#BSUB -o gauss_iterspersec_%j.out
#BSUB -q hpcintro
#BSUB -n 24
#BSUB -R "rusage[mem=10GB]"
#BSUB -W 00:30
# assures that your job has a whole CPU for itself (shared L3 cache)
#BSUB -R "span[hosts=1]"

# Use ⁠ nodestat -F hpcintro ⁠ to figure out what models are available
#BSUB -R "select[model == XeonE5_2650v4]"

OUTPUT_FILE="gauss_iterspersec.data"

# Define grid sizes and tolerances
GRID_SIZES=(10 25 50 75 100 125 150 200 250 300)
TOLERANCES=(1e-3)

# Write header
echo "# Iteration Grid_Size Tolerance Iterations_per_sec Time_per_iteration(s)" > $OUTPUT_FILE

for N in "${GRID_SIZES[@]}"; do
    for T in "${TOLERANCES[@]}"; do
        echo "Running Sequential Gauss-Seidel for Grid Size: $N with Tolerance: $T"

        # Run Gauss-Seidel with version 3
        output=$(./poisson_gs $N 10000 $T 20 3)

        # Extract iteration number and execution time
        iter=$(echo "$output" | awk '/Converged/ {print $3}')
        exec_time=$(echo "$output" | awk '/Exec Time:/ {print $(NF-1)}')

        # Compute iterations per second
        if [[ -n "$iter" && -n "$exec_time" ]]; then
            iter_per_sec=$(awk -v iter="$iter" -v time="$exec_time" 'BEGIN {print iter / time}')
            time_per_iter=$(awk -v iter="$iter" -v time="$exec_time" 'BEGIN {print time / iter}')
            echo "$iter $N $T $iter_per_sec $time_per_iter" >> $OUTPUT_FILE
        fi
    done
done

echo "Data saved to $OUTPUT_FILE"
