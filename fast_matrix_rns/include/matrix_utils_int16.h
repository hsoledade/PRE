#ifndef MATRIX_UTILS_INT16_H
#define MATRIX_UTILS_INT16_H

#include <stdint.h>

/**
 * Allocate a matrix of int16_t with n rows and m columns.
 */
int16_t** allocate_matrix_int16(int n, int m);

/**
 * Free a matrix of int16_t.
 */
void free_matrix_int16(int16_t** mat, int n);

/**
 * Print a matrix of int16_t to stdout.
 */
void print_matrix_int16(int16_t** mat, int n, int m);

#endif // MATRIX_UTILS_INT16_H
