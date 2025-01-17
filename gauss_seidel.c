/* gauss_seidel.c - Poisson problem in 3d
 *
 */
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

double gauss_seidel(double ***f, double ***u, int N, int iter_max, double *tolerance) {
    double h = 2.0 / N;
    double h2 = h * h;
    double diff = 0.0;
    double start_time = omp_get_wtime();  // Start timing
    
    for (int iter = 0; iter < iter_max; iter++) {
        diff = 0.0;

        for (int i = 1; i < N + 1; i++) {
            for (int j = 1; j < N + 1; j++) {
                for (int k = 1; k < N + 1; k++) {
                    // Save old value
                    double old_u = u[i][j][k];

                    // Compute Gauss-Seidel update
                    u[i][j][k] = (1.0 / 6.0) * (
                        u[i - 1][j][k] + u[i + 1][j][k] +
                        u[i][j - 1][k] + u[i][j + 1][k] +
                        u[i][j][k - 1] + u[i][j][k + 1] +
                        h2 * f[i][j][k]
                    );

                    // Compute squared difference for convergence check
                    diff += (u[i][j][k] - old_u) * (u[i][j][k] - old_u);
                }
            }
        }

        // Normalize difference
        diff = sqrt(diff / (N * N * N));

        // Logging iteration progress
        printf("Iteration %d with mean squared difference = %.6f\n", iter + 1, diff);

        // Stop if converged
        if (diff < *tolerance) {
            printf("Converged after %d iterations with mean squared difference = %.6f\n", iter + 1, diff);
            printf("Final Iteration: %d\n", iter + 1);  // Print final iteration
            printf("Execution Time: %.6f seconds\n", omp_get_wtime() - start_time);  // Print execution time
            break;
        }
    }

    return diff;
}

void gauss_seidel_parallel(int N, int iter_max, double ***f, double ***u) {
    double h = 2.0 / N;
    double h2 = h * h;

    for (int iter = 0; iter < iter_max; iter++) {
        #pragma omp parallel for schedule(static,1) ordered(2) 
        for (int i = 1; i < N + 1; i++) {
            for (int j = 1; j < N + 1; j++) {
                #pragma omp ordered depend(sink: i-1, j) depend(sink: i, j-1)
                for (int k = 1; k < N + 1; k++) {
                    u[i][j][k] = (1.0 / 6.0) * (
                        u[i - 1][j][k] + u[i + 1][j][k] +
                        u[i][j - 1][k] + u[i][j + 1][k] +
                        u[i][j][k - 1] + u[i][j][k + 1] +
                        h2 * f[i][j][k]
                    );
                }
                #pragma omp ordered depend(source)
            }
        }
    }

    printf("Completed %d iterations.\n", iter_max);
}

