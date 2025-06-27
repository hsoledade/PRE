#ifndef FILE_IO_INT8_H
#define FILE_IO_INT8_H

#include <stdint.h>

/**
 * Load a matrix of int8_t from a text file.
 */
int8_t** load_matrix_int8(const char* filename, int* out_n, int* out_m);

/**
 * Save a matrix of int8_t to a text file.
 */
void save_matrix_int8(const char* filename, int8_t** mat, int n, int m);

#endif // FILE_IO_INT8_H
