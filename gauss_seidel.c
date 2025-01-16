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

void gauss_seidel(double ***f, double ***u, int N, int iter_max, double *tolerance) {
    double h = 2.0 / (N + 1);
    double h2 = h * h;
    double gauss_scale = 1.0 / 6.0;
    double norm_scale = 1.0 / (N * N * N); // Normalization factor for convergence check
    double delta2 = h2;
    
    int i, j, k, iteration = 0;
    double convergence = INFINITY;

    // Gauss-Seidel iterative solver
    while (convergence > *tolerance && iteration < iter_max) {
        convergence = 0.0;

        for (i = 1; i <= N; i++) {
            for (j = 1; j <= N; j++) {
                for (k = 1; k <= N; k++) {
                    // Compute updated value using Gauss-Seidel formula
                    double new_u = gauss_scale * (
                        u[i - 1][j][k] + u[i + 1][j][k] +
                        u[i][j - 1][k] + u[i][j + 1][k] +
                        u[i][j][k - 1] + u[i][j][k + 1] +
                        delta2 * f[i][j][k]
                    );

                    // Compute difference for convergence check
                    double diff = new_u - u[i][j][k];
                    convergence += diff * diff;

                    // Update u directly (Gauss-Seidel property)
                    u[i][j][k] = new_u;
                }
            }
        }

        // Normalize convergence
        convergence = sqrt(norm_scale * convergence);
        iteration++;

        // Logging iteration progress
        printf("Iteration: %d, Convergence Difference: %.6f\n", iteration, convergence);
    }

    // Final output
    printf("Gauss-Seidel converged after %d iterations with final difference: %.6f\n", iteration, convergence);

    // return convergence;
}

