#!/bin/bash
#BSUB -J jacobi_speedup
#BSUB -o jacobi_speedup_%j.out
#BSUB -q hpc
#BSUB -n 24
#BSUB -R "rusage[mem=10GB]"
#BSUB -W 00:59
#BSUB -R "span[hosts=1]"

#BSUB -R "select[model == XeonE5_2650v4]"
OUTPUT_FILE="speedup_threads_grids_amdahl.data"

# Define different grid sizes and thread counts
GRID_SIZES=(100 200 300)
THREADS=(1 2 4 8 16)
PLACEMENTS=("none" "close" "spread")  # Added "spread" as a new option

echo "# Grid_Size Threads Placement Version Execution_Time Speedup" > $OUTPUT_FILE

for N in "${GRID_SIZES[@]}"; do
    for P in "${PLACEMENTS[@]}"; do
        for T in "${THREADS[@]}"; do
            export OMP_NUM_THREADS=$T

            # Set thread binding based on the placement strategy
            if [[ "$P" == "none" ]]; then
                unset OMP_PROC_BIND 
            elif [[ "$P" == "close" ]]; then
                export OMP_PROC_BIND=close
            elif [[ "$P" == "spread" ]]; then
                export OMP_PROC_BIND=spread
            fi

            for VERSION in 1 2; do  # Run both parallel versions
                echo "Running Grid Size: $N with $T threads (Placement: $P, Version: $VERSION)"

                output=$(./poisson_j $N 1000 1e-6 20 $VERSION)

                # Debugging: Print full output
                echo "Output for Grid Size: $N, Threads: $T, Placement: $P, Version: $VERSION"
                echo "$output"

                # Extract execution time using `grep` for robustness
                if [[ "$VERSION" -eq 1 ]]; then
                    execution_time=$(echo "$output" | grep "Execution_Time_Parallel" | awk '{print $(NF-1)}')
                else
                    execution_time=$(echo "$output" | grep "Execution_Time_Optimized" | awk '{print $(NF-1)}')
                fi

                if [[ -n "$execution_time" ]]; then
                    # Store baseline time for single-threaded execution
                    if [[ "$T" -eq 1 ]]; then
                        if [[ "$VERSION" -eq 1 ]]; then
                            baseline_parallel=$execution_time
                        else
                            baseline_optimized=$execution_time
                        fi
                    fi

                    # Compute Speedup
                    if [[ "$T" -ne 1 ]]; then
                        if [[ "$VERSION" -eq 1 ]]; then
                            if [[ -n "$baseline_parallel" ]]; then
                                speedup=$(awk -v p1="$baseline_parallel" -v pT="$execution_time" 'BEGIN {print p1 / pT}')
                            else
                                speedup="N/A"
                            fi
                        else
                            if [[ -n "$baseline_optimized" ]]; then
                                speedup=$(awk -v p1="$baseline_optimized" -v pT="$execution_time" 'BEGIN {print p1 / pT}')
                            else
                                speedup="N/A"
                            fi
                        fi
                    else
                        speedup="N/A"
                    fi
                else
                    execution_time="N/A"
                    speedup="N/A"
                fi

                echo "$N $T $P $VERSION $execution_time $speedup" >> $OUTPUT_FILE
            done
        done
    done
done

echo "Data saved to $OUTPUT_FILE"
