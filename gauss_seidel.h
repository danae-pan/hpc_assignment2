/* gauss_seidel.h - Poisson problem
 *
 */
#ifndef _GAUSS_SEIDEL_H
#define _GAUSS_SEIDEL_H

// define your function prototype here
double gauss_seidel(double ***f, double ***u, int N, int iter_max, double *tolerance);
void gauss_seidel_parallel(int N, int iter_max, double ***f, double ***u);
#endif
