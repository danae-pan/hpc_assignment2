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
    double h = 2.0 / N;
    double h2 = h * h;
    double diff = 0.0;

    for (int iter = 0; iter < iter_max; iter++)
    {
        diff = 0;
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
                    diff += (u_new[i][j][k] - u[i][j][k]) * (u_new[i][j][k] - u[i][j][k]);
                }
            }
        }

        diff = sqrt(diff / (N * N * N));
        printf("Iteration %d with mean squared difference = %.6f\n", iter + 1, diff);

        // Stop if converged
        if (diff < *tolerance)
        {
            printf("Converged after %d iterations with mean squared difference = %.6f\n", iter + 1, diff);
            break;
        }

        // Swap pointers
        double ***temp = u;
        u = u_new;
        u_new = temp;
    }

    return diff;
}

int jacobi_parallel_opt(double ***f, double ***u, double ***u_new, int N, int iter_max, double *tolerance, double* final_diff)
{
    double h = 2.0 / N;
    double h2 = h * h;
    double diff = 0.0;
    int iter;

    for  (iter = 1; iter <= iter_max; iter++)
    {
        diff = 0.0;
        #pragma omp parallel for shared(f, u, u_new, h2) reduction(+:diff) schedule(static)
        for (int i = 1; i <= N; i++)
        {
            for (int j = 1; j <= N; j++)
            {
                for (int k = 1; k <= N; k++)
                {
                    double temp = (1.0 / 6.0) * (u[i - 1][j][k] + u[i + 1][j][k] +
                                                 u[i][j - 1][k] + u[i][j + 1][k] +
                                                 u[i][j][k - 1] + u[i][j][k + 1] +
                                                 h2 * f[i][j][k]);

                    double local_diff = temp - u[i][j][k];  // Compute difference

                    // Update grid point
                    u_new[i][j][k] = temp;

                    // Accumulate squared differences for RMS computation
                    diff += local_diff * local_diff;
                }
            }
        }

        // Compute RMS norm
        diff = sqrt(diff / (N * N * N));

        // Stop if converged
        if (diff < *tolerance)
        {
            printf("Converged after %d iterations with RMS diff = %.6f\n", iter, diff);
            *final_diff = diff;  // Store final diff value
            return iter;
        }

        // Swap u and u_new
        double ***temp = u;
        u = u_new;
        u_new = temp;
    }

    *final_diff = diff;  // Store final diff value even if max_iter is reached
    return iter_max; // Return max_iter if no convergence
}


int jacobi_parallel(double ***f, double ***u, double ***u_new, int N, int iter_max, double *tolerance, double* final_diff)
{
    //printf("DEBUG: Inside jacobi_parallel(), starting computation...\n"); // <-- Add this
    double h = 2.0 / N;
    double h2 = h * h;
    double diff = 0.0;

    for (int iter = 1; iter <= iter_max; iter++)
    {
        //printf("DEBUG: Iteration %d\n", iter); // <-- Track iterations

        diff = 0.0;
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
                }
            }
        }

        //printf("DEBUG: Finished parallel update, computing diff...\n"); // <-- Verify OpenMP execution

        // Compute difference
        diff = 0.0;
        for (int i = 1; i <= N; i++)
        {
            for (int j = 1; j <= N; j++)
            {
                for (int k = 1; k <= N; k++)
                {
                    double local_diff = (u_new[i][j][k] - u[i][j][k]) * (u_new[i][j][k] - u[i][j][k]);
                    diff += local_diff;
                }
            }
        }

        diff = sqrt(diff / (N * N * N));

        //printf("DEBUG: Iteration %d complete, diff = %.6f\n", iter, diff); // <-- Ensure convergence calculation

        // Stop if converged
        if (diff < *tolerance)
        {
            printf("Converged after %d iterations with diff = %.6f\n", iter, diff);
            *final_diff = diff;
            return iter;
        }

        // Swap u and u_new
        double ***temp = u;
        u = u_new;
        u_new = temp;
    }

    //printf("DEBUG: Exiting jacobi_parallel()...\n"); // <-- Ensure function exits correctly
    *final_diff = diff;
    return iter_max;
}