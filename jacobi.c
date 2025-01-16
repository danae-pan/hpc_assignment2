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

double jacobi_parallel(double ***f, double ***u, double ***u_new, int N, int iter_max, double *tolerance)
{
    double h = 2.0 / (N);
    double h2 = h * h;
    double diff = 0.0;

    for (int iter = 1; iter <= iter_max; iter++)
    {
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
                    diff += (u_new[i][j][k] - u[i][j][k]) * (u_new[i][j][k] - u[i][j][k]);
                }
            }
        }

        diff = sqrt(diff / (N * N * N));
        printf(" with max_diff = %.6f\n",  diff);
        // Stop if converged
        if (diff < *tolerance)
        {
            printf("Converged after %d iterations with max_diff = %.6f\n", iter + 1, diff);
            break;
        }

        // Swap u and u_new
        double ***temp = u;
        u = u_new;
        u_new = temp;

        
    }
    return diff;
}


double jacobi_parallel_opt(double ***f, double ***u, double ***u_new, int N, int iter_max, double *tolerance)
{
    double h = 2.0 / N;
    double h2 = h * h;
    double diff = 0.0;

    for (int iter = 1; iter <= iter_max; iter++)
    {
        // Update u_new values
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

        // Compute difference
        diff = 0.0;
        #pragma omp parallel for reduction(+:diff) schedule(static)
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
        printf("Iteration %d with mean squared difference = %.6f\n", iter, diff);

        // Stop if converged
        if (diff < *tolerance)
        {
            printf("Converged after %d iterations with mean squared difference = %.6f\n", iter, diff);
            break;
        }

        // Swap u and u_new
        double ***temp = u;
        u = u_new;
        u_new = temp;
    }
    return diff;
}