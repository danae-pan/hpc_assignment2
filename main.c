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
int jacobi_parallel(double ***f, double ***u, double ***u_new, int N, int iter_max, double *threshold, double* final_diff);
#endif

#ifdef _JACOBI_PARALLEL_OPT
int jacobi_parallel_opt(double ***f, double ***u, double ***u_new, int N, int iter_max, double *threshold, double* final_diff);
#endif

#endif

int main(int argc, char *argv[])
{
    int N = 100, max_iter = 1000, version = 0; // Default values
    double threshold = 1e-6, start_T = 20.0, delta;
    double ***f = NULL, ***u = NULL, ***u_new = NULL;
    double error = 0.0;
    double final_diff = 0.0;
    int iterations = 0;
    double start_time, end_time;

    // Validate Command-line Arguments
    if (argc < 6)
    {
        fprintf(stderr, "Usage: %s <grid_size> <max_iterations> <threshold> <start_temperature> <version>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Read Inputs
    N = atoi(argv[1]);        // Grid size
    max_iter = atoi(argv[2]); // Maximum iterations
    threshold = atof(argv[3]); // Convergence threshold
    start_T = atof(argv[4]);   // (Unused in solver, but passed)
    version = atoi(argv[5]);   // Jacobi version selector

    // Debugging: Print Parsed Inputs
    printf("Parsed Arguments:\n");
    printf("N = %d, iter_max = %d, tolerance = %.6f, start_T = %.2f, version = %d\n",
           N, max_iter, threshold, start_T, version);

    // Allocate memory for 3D arrays
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

                // Define heat source (Radiator)
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

    // Measure Execution Time
    start_time = omp_get_wtime();

    // Run Selected Jacobi Version
    //printf("Switching to Jacobi version %d...\n", version);  // Debugging output
    switch (version)
    {
#ifdef _JACOBI
    case 0:
        printf("Running Sequential Jacobi...\n");
        error = jacobi(f, u, u_new, N, max_iter, &threshold);
        break;
#endif

#ifdef _JACOBI_PARALLEL
    case 1:
        printf("Running Parallel Simple Jacobi...\n");
        final_diff = 0.0;
        iterations = jacobi_parallel(f, u, u_new, N, max_iter, &threshold, &final_diff);
        break;
#endif

#ifdef _JACOBI_PARALLEL_OPT
    case 2:
        printf("Running Parallel Optimized Jacobi...\n");
        final_diff = 0.0;
        iterations = jacobi_parallel_opt(f, u, u_new, N, max_iter, &threshold, &final_diff);
        break;
#endif

    default:
        //printf("DEBUG: Entered default case with version = %d\n", version);
        fprintf(stderr, "Invalid version selected! Use 0 for sequential, 1 for parallel simple, 2 for parallel optimized.\n");
        exit(EXIT_FAILURE);
    }

    end_time = omp_get_wtime();

    // Determine output file name based on program name and version
    char output_file[50];

    if (strstr(argv[0], "poisson_j_opt") != NULL) {
        strcpy(output_file, "jacobi_opt_results.data");  // Optimized results file
    } else {
        // Non-optimized results file based on version
        sprintf(output_file, "jacobi_noopt_results_v%d.data", version);
    }

    // Open the correct data file
    FILE *fp = fopen(output_file, "a");
    if (fp == NULL)
    {
        perror("Failed to open results file");
        exit(EXIT_FAILURE);
    }

    // Append results: Parallel version, Grid size (N), Iterations, Threads, Tolerance, Final Diff, Time(s)
    fprintf(fp, "%d %d %d %d %.6f %.6f %.6f\n", version, N, omp_get_max_threads(), iterations, threshold, final_diff, end_time - start_time);
    fclose(fp);

    printf("Results saved to %s\n", output_file);
    // Free Memory
    free_3d(f);
    free_3d(u);
    free_3d(u_new);

    return 0;
}