#ifndef MATRIX_UTILS_INT8_H
#define MATRIX_UTILS_INT8_H

#include <stdint.h>

/**
 * Allocate a matrix of int8_t with n rows and m columns.
 */
int8_t** allocate_matrix_int8(int n, int m);

/**
 * Free a matrix of int8_t.
 */
void free_matrix_int8(int8_t** mat, int n);

/**
 * Print a matrix of int8_t to stdout.
 */
void print_matrix_int8(int8_t** mat, int n, int m);

#endif // MATRIX_UTILS_INT8_H
