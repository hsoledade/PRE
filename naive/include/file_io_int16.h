#ifndef FILE_IO_INT16_H
#define FILE_IO_INT16_H

#include <stdint.h>

/**
 * Load a matrix of int16_t from a text file.
 */
int16_t** load_matrix_int16(const char* filename, int* out_n, int* out_m);

/**
 * Save a matrix of int16_t to a text file.
 */
void save_matrix_int16(const char* filename, int16_t** mat, int n, int m);

#endif // FILE_IO_INT16_H
