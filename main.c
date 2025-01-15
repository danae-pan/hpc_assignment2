/* main.c - Poisson problem in 3D
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include "alloc3d.h"
#include "print.h"
#include <math.h>


#include "jacobi.h"


#ifdef _GAUSS_SEIDEL
#include "gauss_seidel.h"
#endif



#define N_DEFAULT 100

int
main(int argc, char *argv[]) {

    int 	N = N_DEFAULT;
    int 	iter_max = 1000;
    double	tolerance;
    double	start_T;
    int		output_type = 0;
    char	*output_prefix = "poisson_res";
    char        *output_ext    = "";
    char	output_filename[FILENAME_MAX];
    double 	***u = NULL;


    /* get the paramters from the command line */
    N         = atoi(argv[1]);	// grid size
    iter_max  = atoi(argv[2]);  // max. no. of iterations
    tolerance = atof(argv[3]);  // tolerance
    start_T   = atof(argv[4]);  // start T for all inner grid points
    if (argc == 6) {
	output_type = atoi(argv[5]);  // ouput type
    }

    // allocate memory
    if ( (u = malloc_3d(N, N, N)) == NULL ) {
        perror("array u: allocation failed");
        exit(-1);
    }


    // Dynamic allocation
    double ***A = malloc_3d(N, N, N);
    if (A == NULL) {
        perror("Allocation for A failed");
        exit(EXIT_FAILURE);
    }

    double ***b = malloc_3d(N, N, N); 
    if (b == NULL) {
        perror("Allocation for b failed");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            for (int k = 0; k < N; k++) {
                A[i][j][k] = (i == j && j == k) ? 2.0 : 0.1; // Example initialization
                b[i][j][k] = 1.0;                           // Example initialization
            }
        }
    }

    for (int i = 0; i < N; i++) {
    for (int j = 0; j < N; j++) {
        for (int k = 0; k < N; k++) {
            if (i == 0 || i == N - 1 || j == 0 || j == N - 1 || k == 0 || k == N - 1) {
                u[i][j][k] = (j == 0) ? 0.0 : 20.0; // 0°C on one wall, 20°C on others
            } else if (i >= (N / 8) && i <= (3 * N / 8) && 
                       j >= (N / 8) && j <= (N / 2) && 
                       k >= (N / 3) && k <= (2 * N / 3)) {
                u[i][j][k] = 200.0; // Radiator
            } else {
                u[i][j][k] = 0.0; // Initial temperature for other points
            }
        }
    }
}


    int iterations = 0;
    double error;

do {
    #ifdef _JACOBI
    jacobi(A, b, u, N, iter_max, &tolerance);
    #endif

    error = 0.0;
    for (int i = 1; i < N - 1; i++) {
        for (int j = 1; j < N - 1; j++) {
            for (int k = 1; k < N - 1; k++) {
                double residual = b[i][j][k];
                for (int l = 0; l < N; l++) {
                    residual -= A[i][j][k] * u[l][j][k];
                }
                error += residual * residual;
            }
        }

    }
    error = sqrt(error);
    
    printf("Iteration: %d, Error: %.6f\n", iterations + 1, error);

    iterations++;
} while (error > tolerance && iterations < iter_max);


   
    /*
     *
     * fill in your code here 
     *
     *
     */

    // dump  results if wanted 
    switch(output_type) {
	case 0:
	    // no output at all
	    break;
	case 3:
	    output_ext = ".bin";
	    sprintf(output_filename, "%s_%d%s", output_prefix, N, output_ext);
	    fprintf(stderr, "Write binary dump to %s: ", output_filename);
	    print_binary(output_filename, N, u);
	    break;
	case 4:
	    output_ext = ".vtk";
	    sprintf(output_filename, "%s_%d%s", output_prefix, N, output_ext);
	    fprintf(stderr, "Write VTK file to %s: ", output_filename);
	    print_vtk(output_filename, N, u);
	    break;
	default:
	    fprintf(stderr, "Non-supported output type!\n");
	    break;
    }

    // de-allocate memory
    free_3d(u);
    // Free memory
    free_3d(A);
    free_3d(b);
    return(0);
}
