#ifndef MATRIX_UTILS_H
#define MATRIX_UTILS_H

/**
 * Allocates a dynamic 2D integer matrix of size n × m.
 * @param n Number of rows
 * @param m Number of columns
 * @return Pointer to allocated matrix (int**)
 */
int** allocate_matrix(int n, int m);

/**
 * Frees a dynamically allocated 2D matrix of integers.
 * @param mat Pointer to matrix
 * @param n Number of rows
 */
void free_matrix(int** mat, int n);

/**
 * Prints a 2D integer matrix to the standard output.
 * @param mat Pointer to matrix
 * @param n Number of rows
 * @param m Number of columns
 */
void print_matrix(int** mat, int n, int m);

#endif // MATRIX_UTILS_H
