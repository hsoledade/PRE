#ifndef MATRIX_UTILS_GMP_H
#define MATRIX_UTILS_GMP_H

#include <gmp.h>

/**
 * Allocates a dynamic 2D matrix of mpz_t (multi-precision integers) of size n × m.
 * Initializes each element with mpz_init.
 * 
 * @param n Number of rows
 * @param m Number of columns
 * @return Pointer to the allocated matrix (mpz_t**)
 */
mpz_t** allocate_mpz_matrix(int n, int m);

/**
 * Frees a dynamically allocated 2D matrix of mpz_t elements.
 * Calls mpz_clear on each element before freeing memory.
 * 
 * @param mat Pointer to the matrix
 * @param n Number of rows
 * @param m Number of columns
 */
void free_mpz_matrix(mpz_t** mat, int n, int m);

/**
 * Prints a 2D matrix of mpz_t elements to the standard output.
 * 
 * @param mat Pointer to the matrix
 * @param n Number of rows
 * @param m Number of columns
 */
void print_mpz_matrix(mpz_t** mat, int n, int m);

#endif // MATRIX_UTILS_GMP_H
