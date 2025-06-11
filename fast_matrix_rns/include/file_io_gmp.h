#ifndef FILE_IO_GMP_H
#define FILE_IO_GMP_H

#include <gmp.h>

/**
 * Reads a matrix of mpz_t from a text file.
 * 
 * @param filename Path to the input file
 * @param n Pointer to store number of rows
 * @param m Pointer to store number of columns
 * @return Pointer to the allocated and filled mpz_t matrix
 */
mpz_t** read_mpz_matrix_from_file(const char* filename, int* n, int* m);

/**
 * Writes a matrix of mpz_t to a text file.
 * 
 * @param filename Path to the output file
 * @param mat Pointer to the matrix
 * @param n Number of rows
 * @param m Number of columns
 */
void write_mpz_matrix_to_file(const char* filename, mpz_t** mat, int n, int m);

#endif // FILE_IO_GMP_H
