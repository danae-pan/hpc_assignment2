#include <stdio.h>
#include <stdlib.h>
#include "alloc3d.h"
#include "print.h"
#include <math.h>

#ifdef _JACOBI
#include "jacobi.h"
#endif

#define N_DEFAULT 100

int main(int argc, char *argv[])
{
    int N = N_DEFAULT;
    int iter_max = 1000;
    double tolerance;
    double start_T;
    int output_type = 0;
    char *output_prefix = "poisson_res";
    char *output_ext = "";
    char output_filename[FILENAME_MAX];
    double ***u = NULL, ***f = NULL, ***u_new = NULL;

    if (argc < 5)
    {
        fprintf(stderr, "Usage: %s <grid_size> <max_iterations> <tolerance> <start_temperature> [output_type]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    N = atoi(argv[1]);
    iter_max = atoi(argv[2]);
    tolerance = atof(argv[3]);
    start_T = atof(argv[4]);
    if (argc == 6)
    {
        output_type = atoi(argv[5]);
    }

    if ((u = malloc_3d(N + 2, N + 2, N + 2)) == NULL ||
        (f = malloc_3d(N + 2, N + 2, N + 2)) == NULL ||
        (u_new = malloc_3d(N + 2, N + 2, N + 2)) == NULL)
    {
        perror("Memory allocation failed");
        free_3d(u);
        free_3d(f);
        free_3d(u_new);
        exit(EXIT_FAILURE);
    }

    double h = 2.0 / (N + 1);
    for (int i = 0; i < N + 2; i++)
    {
        for (int j = 0; j < N + 2; j++)
        {
            for (int k = 0; k < N + 2; k++)
            {
                if (i >= N / 2 && i <= 2 * N / 3 &&
                    j >= N / 6 && j <= N / 3 &&
                    k >= N / 3 && k <= 2 * N / 3)
                {

                    f[i][j][k] = 200.0;
                    // printf("f[%d][%d][%d] = %.6f\n", i, j, k, f[i][j][k]);
                }
                else
                {
                    f[i][j][k] = 0.0;
                }

                if (i == 0 || i == N + 1 || j == 0 || j == N + 1 || k == 0 || k == N + 1)
                {
                    u[i][j][k] = 20.0;
                }
                else
                {
                    u[i][j][k] = start_T;
                }

                u_new[i][j][k] = u[i][j][k];
            }
        }
    }

    int iterations = 0;
    double error;

    do
    {

#ifdef _JACOBI
        error = jacobi(f, u, u_new, N, iter_max, &tolerance); // Perform one iteration at a time.
#endif
        iterations++;

        printf("Iteration %d: max_diff = %.6f \n", iterations, error);

    } while (error > tolerance && iterations < iter_max);
    switch (output_type)
    {
    case 0:
        break;
    case 3:
        output_ext = ".bin";
        sprintf(output_filename, "%s_%d%s", output_prefix, N, output_ext);
        print_binary(output_filename, N + 2, u);
        break;
    case 4:
        output_ext = ".vtk";
        sprintf(output_filename, "%s_%d%s", output_prefix, N, output_ext);
        print_vtk(output_filename, N + 2, u);
        break;
    default:
        fprintf(stderr, "Non-supported output type!\n");
        break;
    }

    free_3d(u);
    free_3d(f);
    free_3d(u_new);

    return 0;
}
