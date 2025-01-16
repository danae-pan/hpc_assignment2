/* gauss_seidel.c - Poisson problem in 3d
 *
 */
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

/* double gauss_seidel(double ***f, double ***u, int N, int iter_max, double *tolerance) {
    double h = 2.0 / (N + 1);
    double h2 = h * h;
    double max_diff = 0.0;

    for (int iter = 0; iter < iter_max; iter++) {
        max_diff = 0.0;

        for (int i = 1; i <= N; i++) {
            for (int j = 1; j <= N; j++) {
                for (int k = 1; k <= N; k++) {
                    // Save the old value
                    double old_u = u[i][j][k];

                    // Update using Gauss-Seidel formula
                    u[i][j][k] = (1.0 / 6.0) * (
                        u[i - 1][j][k] + u[i + 1][j][k] +
                        u[i][j - 1][k] + u[i][j + 1][k] +
                        u[i][j][k - 1] + u[i][j][k + 1] +
                        h2 * f[i][j][k]
                    );

                    // Calculate the difference
                    double diff = fabs(u[i][j][k] - old_u);
                    if (diff > max_diff) {
                        max_diff = diff;
                    }
                }
            }
        }
        printf("\tIterations: %d\n", iter);
        printf("\tConvergence_difference: %lf\n", max_diff);

        // Check for convergence
        if (max_diff < *tolerance) {
            printf("Converged after %d iterations with max_diff = %.6f\n", iter + 1, max_diff);
            break;
        }
    }

    return max_diff;
}*/

double gauss_seidel(double ***f, double ***u, int N, int iter_max, double *tolerance) {
    double h = 2.0 / N;
    double h2 = h * h;
    double diff = 0.0;
    
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
            break;
        }
    }

    return diff;
}
