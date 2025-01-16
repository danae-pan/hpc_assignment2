/* jacobi.c - Poisson problem in 3d
 *
 */
#include "jacobi.h"
#include <stdlib.h>
#include <stdio.h>
#include "alloc3d.h"
#include <math.h>

#include <omp.h>

double jacobi(double ***f, double ***u, double ***u_new, int N, int iter_max, double *tolerance)
{
    double h = 2.0 / (N + 1);
    double h2 = h * h;
    double max_diff = 0.0;

    for (int iter = 0; iter < iter_max; iter++)
    {
        for (int i = 1; i < N + 1; i++)
        {
            for (int j = 1; j < N + 1; j++)
            {
                for (int k = 1; k < N + 1; k++)
                {
                    u_new[i][j][k] = (1.0 / 6.0) * (u[i - 1][j][k] + u[i + 1][j][k] +
                                                    u[i][j - 1][k] + u[i][j + 1][k] +
                                                    u[i][j][k - 1] + u[i][j][k + 1] +
                                                    h2 * f[i][j][k]);
                }
            }
        }

        for (int i = 1; i < N + 1; i++)
        {
            for (int j = 1; j < N + 1; j++)
            {
                for (int k = 1; k < N + 1; k++)
                {
                    double diff = fabs(u_new[i][j][k] - u[i][j][k]);
                    if (diff > max_diff)
                    {
                        max_diff = diff;
                    }
                }
            }
        }

        // Stop if converged
        if (max_diff < *tolerance)
        {
            printf("Converged after %d iterations with max_diff = %.6f\n", iter + 1, max_diff);
            break;
        }

        double ***temp = u;
        u = u_new;
        u_new = temp;
    }

    return max_diff;
}

double jacobi_parallel(double ***f, double ***u, double ***u_new, int N, int iter_max, double *tolerance)
{
    double h = 2.0 / (N + 1);
    double h2 = h * h;
    double max_diff = 0.0;

    for (int iter = 0; iter < iter_max; iter++)
    {
        max_diff = 0.0;
#pragma omp parallel for shared(f, u, u_new, h2) schedule(static)
        for (int i = 1; i <= N; i++)
        {
            for (int j = 1; j <= N; j++)
            {
                for (int k = 1; k <= N; k++)
                {
                    u_new[i][j][k] = (1.0 / 6.0) * (u[i - 1][j][k] + u[i + 1][j][k] +
                                                    u[i][j - 1][k] + u[i][j + 1][k] +
                                                    u[i][j][k - 1] + u[i][j][k + 1] +
                                                    h2 * f[i][j][k]);
                    double diff = fabs(u_new[i][j][k] - u[i][j][k]);
                    if (diff > max_diff)
                    {
                        max_diff = diff;
                    }
                }
            }
        }

        // Stop if converged
        if (max_diff < *tolerance)
        {
            printf("Converged after %d iterations with max_diff = %.6f\n", iter + 1, max_diff);
            break;
        }

        // Swap u and u_new
        double ***temp = u;
        u = u_new;
        u_new = temp;
    }

    return max_diff;
}

double jacobi_parallel_opt(double ***f, double ***u, double ***u_new, int N, int iter_max, double *tolerance) {
    double h = 2.0 / (N + 1);
    double h2 = h * h;
    double max_diff = 0.0;

    for (int iter = 0; iter < iter_max; iter++) {
        max_diff = 0.0;

        #pragma omp parallel for collapse(2) shared(f, u, u_new, h2) schedule(static)
        for (int i = 1; i <= N; i++) {
            for (int j = 1; j <= N; j++) {
                for (int k = 1; k <= N; k++) {
                    double updated_value = (1.0 / 6.0) * (
                        u[i-1][j][k] + u[i+1][j][k] +
                        u[i][j-1][k] + u[i][j+1][k] +
                        u[i][j][k-1] + u[i][j][k+1] +
                        h2 * f[i][j][k]
                    );
                    double diff = fabs(updated_value - u[i][j][k]);
                    
                    if (diff > max_diff) {
                        max_diff = diff;  // Needs to be inside reduction
                    }

                    u_new[i][j][k] = updated_value;
                }
            }
        }

        // Stop if converged
        if (max_diff < *tolerance) {
            printf("Converged after %d iterations with max_diff = %.6f\n", iter + 1, max_diff);
            break;
        }

        // Swap u and u_new
        double ***temp = u;
        u = u_new;
        u_new = temp;
    }

    return max_diff;
}
