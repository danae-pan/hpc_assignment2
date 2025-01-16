#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
#include "alloc3d.h"
#include "jacobi.h"
#include "print.h"


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

#ifndef _GAUSS_SEIDEL_H
#define _GAUSS_SEIDEL_H

void gauss_seidel(double ***f, double ***u, int N, int iter_max, double *threshold);

#endif

int main(int argc, char *argv[])
{
    int N = 100, max_iter = 1000, version = 0; // Default values
    double threshold = 1e-6, start_T = 20.0, delta;
    double ***f = NULL, ***u = NULL, ***u_new = NULL;
    double max_diff = 0.0;
    double error = 0.0;
    int iterations = 0;
    int		output_type = 0;
    char	*output_prefix = "poisson_res";
    char        *output_ext    = "";
    char	output_filename[FILENAME_MAX];
    double start_time, end_time; 

    // Parse command-line arguments
    if (argc < 6)
    {
        fprintf(stderr, "Usage: %s <grid_size> <max_iterations> <threshold> <start_temperature> [version] [output]\n", argv[0]);
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
    if (argc == 7) {
	output_type = atoi(argv[6]);  // ouput type
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
        do
        {
            max_diff = jacobi(f, u, u_new, N, max_iter, &threshold);
            iterations++;
        } while (max_diff > threshold && iterations < max_iter);
        break;
#endif

#ifdef _JACOBI_PARALLEL
    case 1:
        printf("Running Parallel Simple Jacobi...\n");
        
        error = jacobi_parallel(f, u, u_new, N, max_iter, &threshold);
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
#ifdef _GAUSS_SEIDEL
    case 3:
        printf("Running Sequential Gauss-Seidel...\n");
        gauss_seidel(f, u, N, max_iter, &threshold);
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

    switch(output_type) {
	case 0:
	    // no output at all
	    break;
	case 3:
	    output_ext = ".bin";
	    sprintf(output_filename, "%s_%d%s", output_prefix, N, output_ext);
	    fprintf(stderr, "Write binary dump to %s: ", output_filename);
	    print_binary(output_filename, N+2, u);
	    break;
	case 4:
	    output_ext = ".vtk";
	    sprintf(output_filename, "%s_%d%s", output_prefix, N, output_ext);
	    fprintf(stderr, "Write VTK file to %s: ", output_filename);
	    print_vtk(output_filename, N+2, u);
	    break;
	default:
	    fprintf(stderr, "Non-supported output type!\n");
	    break;
    }

    // Free memory
    free_3d(f);
    free_3d(u);
    free_3d(u_new);

    return 0;
}
