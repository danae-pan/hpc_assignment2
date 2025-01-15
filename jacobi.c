/* jacobi.c - Poisson problem in 3d
 * 
 */
#include "jacobi.h"
#include <stdlib.h>
#include <stdio.h>
#include "alloc3d.h"

int jacobi(double ***A, double ***b, double ***u, int N, int iter_max, double *tolerance) {
    double ***u_new = malloc_3d(N, N, N);
    if (u_new == NULL) {
        perror("Allocation for u_new failed");
        return -1;
    }

    for (int i = 1; i < N - 1; i++) {
        for (int j = 1; j < N - 1; j++) {
            for (int k = 1; k < N - 1; k++) {
                double sum = 0.0;
                for (int l = 0; l < N; l++) {
                    if (l != i) {
                        sum += A[i][j][k] * u[l][j][k];
                    }
                }
                u_new[i][j][k] = (b[i][j][k] - sum) / A[i][i][k];
            }
        }
    }

    // Copy u_new back into u
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            for (int k = 0; k < N; k++) {
                u[i][j][k] = u_new[i][j][k];
            }
        }
    }

    free_3d(u_new);
    return 0; 
}
