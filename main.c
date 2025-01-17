#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
#include "alloc3d.h"
#include "jacobi.h"
#include "gauss_seidel.h"
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

double gauss_seidel(double ***f, double ***u, int N, int iter_max, double *threshold);

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
    version = atoi(argv[5]); // Select Jacobi version
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
for (int i = 0; i < N + 2; i++) {
    for (int j = 0; j < N + 2; j++) {
        for (int k = 0; k < N + 2; k++) {
            double x = -1.0 + i * delta;
            double y = -1.0 + j * delta;
            double z = -1.0 + k * delta;

            // Set initial temperatures
            if (i == 0 || i == N + 1 || k == 0 || k == N + 1 || j == N + 1) {
                u[i][j][k] = 20.0;  // Five walls at 20°C
            } else if (j == 0) {
                u[i][j][k] = 0.0;  // The bottom wall at 0°C
            } else {
                u[i][j][k] = 0.0;  // Inside the domain
            }
            u_new[i][j][k] = u[i][j][k];

            // Initialize f for the radiator
            if (x >= -1.0 && x <= -3.0/8.0 &&
                y >= -1.0 && y <= -0.5 &&
                z >= -2.0/3.0 && z <= 0.0) {
                f[i][j][k] = 200.0;  // Radiator heat source
            } else {
                f[i][j][k] = 0.0;  // No heat source elsewhere
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
        start_time = omp_get_wtime();  // Start Timer
        error = gauss_seidel(f, u, N, max_iter, &threshold);
        end_time = omp_get_wtime();  // End Timer
        printf("Execution Time (Sequential Gauss-Seidel): %.6f seconds\n", end_time - start_time);
        break;
#endif

#ifdef _GAUSS_SEIDEL_PARALLEL
    case 4:
        printf("Running Parallel Gauss-Seidel...\n");
        start_time = omp_get_wtime();  // Start Timer
        gauss_seidel_parallel(N, max_iter, f, u);
        end_time = omp_get_wtime();  // End Timer
        printf("Execution Time (Parallel Gauss-Seidel): %.6f seconds\n", end_time - start_time);
        break;
#endif

    default:
        fprintf(stderr, "Invalid version selected! Use 0 for sequential, 1 for parallel simple, 2 for parallel optimized.\n");
        exit(EXIT_FAILURE);
    }

    
    end_time = omp_get_wtime();

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
