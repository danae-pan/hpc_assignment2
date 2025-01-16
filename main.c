#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
#include "alloc3d.h"
#include "jacobi.h"


#ifndef _JACOBI_H
#define _JACOBI_H

#ifdef _JACOBI
double jacobi(double ***f, double ***u, double ***u_new, int N, int iter_max, double *threshold);
#endif

#ifdef _JACOBI_PARALLEL
double jacobi_parallel(double ***f, double ***u, double ***u_new, int N, int iter_max, double *threshold);
#endif

#ifdef _JACOBI_PARALLEL_OPT
double jacobi_parallel_opt(double ***f, double ***u, double ***u_new, int N, int iter_max, double *threshold);
#endif

#endif

// Function to measure execution time
double measure_execution_time(double (*jacobi_func)(double ***, double ***, double ***, int, int, double *), double ***f, double ***u, double ***u_new, int N, int iter_max, double *tolerance)
{
    double start_time = omp_get_wtime();
    double error = jacobi_func(f, u, u_new, N, iter_max, tolerance);
    double end_time = omp_get_wtime();
    return end_time - start_time;
}

// Function to compute speedup
#ifdef _JACOBI
void compute_speedup(double ***f, double ***u, double ***u_new, int N, int iter_max, double *tolerance)
{
    printf("Grid Size: %d\n", N);
    double time_parallel = measure_execution_time(jacobi_parallel, f, u, u_new, N, iter_max, tolerance);
    double time_parallel_opt = measure_execution_time(jacobi_parallel_opt, f, u, u_new, N, iter_max, tolerance);

    printf("Execution Time (Parallel): %.6f seconds\n", time_parallel);
    printf("Execution Time (Parallel Optimized): %.6f seconds\n", time_parallel_opt);

    if (time_parallel > 0 && time_parallel_opt > 0)
    {
        printf("Speedup (Parallel vs Optimized): %.2fx\n", time_parallel / time_parallel_opt);
    }
    printf("------------------------------\n");
}
#endif

int main(int argc, char *argv[])
{
    int N = 100, max_iter = 1000, version = 0; // Default values
    double threshold = 1e-6, start_T = 20.0, delta;
    double ***f = NULL, ***u = NULL, ***u_new = NULL;
    double max_diff = 0.0;
    double error = 0.0;
    int iterations = 0;
    double start_time, end_time; 

    // Parse command-line arguments
    if (argc < 5)
    {
        fprintf(stderr, "Usage: %s <grid_size> <max_iterations> <threshold> <start_temperature> [version]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    N = atoi(argv[1]);
    max_iter = atoi(argv[2]);
    threshold = atof(argv[3]);
    start_T = atof(argv[4]);
    if (argc == 6)
    {
        version = atoi(argv[5]); // Select Jacobi version
    }

    // Allocate memory
    if ((f = malloc_3d(N + 2, N + 2, N + 2)) == NULL ||
        (u = malloc_3d(N + 2, N + 2, N + 2)) == NULL ||
        (u_new = malloc_3d(N + 2, N + 2, N + 2)) == NULL)
    {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }

    // Initialize f, u, and u_new
    delta = 2.0 / (N + 1);
    for (int i = 0; i < N + 2; i++)
    {
        for (int j = 0; j < N + 2; j++)
        {
            for (int k = 0; k < N + 2; k++)
            {
                if (i == 0 || i == N + 1 || j == 0 || j == N + 1 || k == 0 || k == N + 1)
                {
                    u[i][j][k] = start_T;
                }
                else
                {
                    u[i][j][k] = 0.0;
                }
                u_new[i][j][k] = u[i][j][k];

                // Initialize f for the radiator as per the problem
                if (i >= N * 0.125 && i <= N * 0.375 &&
                    j >= N * 0.25 && j <= N * 0.5 &&
                    k >= N * 0.333 && k <= N * 0.666)
                {
                    f[i][j][k] = 200.0;
                }
                else
                {
                    f[i][j][k] = 0.0;
                }
            }
        }
    }

    start_time = omp_get_wtime();
    // Select Jacobi version
    switch (version)
    {
#ifdef _JACOBI
    case 0:
        printf("Running Sequential Jacobi...\n");
        error = jacobi(f, u, u_new, N, max_iter, &threshold);
        printf("print %.6f", error);
        break;
#endif

#ifdef _JACOBI_PARALLEL
    case 1:
        printf("Running Parallel Simple Jacobi...\n");
        
        error = jacobi_parallel(f, u, u_new, N, max_iter, &threshold);
        
    
        compute_speedup(f, u, u_new, N, max_iter, &threshold);
        
        printf("print %.6f", error);
        break;
#endif

#ifdef _JACOBI_PARALLEL_OPT
    case 2:
        printf("Running Parallel Optimized Jacobi...\n");
        
        error= jacobi_parallel_opt(f, u, u_new, N, max_iter, &threshold);
        printf("print %.6f", error);
        break;
#endif
    default:
        fprintf(stderr, "Invalid version selected! Use 0 for sequential, 1 for parallel simple, 2 for parallel optimized.\n");
        exit(EXIT_FAILURE);
    }

    
    end_time = omp_get_wtime();

/*  printf("Version: %d, Threads: %d, Total Iterations: %d, Final max_diff: %.6f, Execution Time: %.6f seconds\n",
           version,
           omp_get_max_threads(),
           iterations,
           max_diff,
           end_time - start_time);
           */  

    

    // Free memory
    free_3d(f);
    free_3d(u);
    free_3d(u_new);

    return 0;
}
