#include <stdio.h>
#include <stdlib.h>
#include <gmp.h>
#include "file_io_gmp.h"
#include "matrix_utils_gmp.h"

/**
 * Reads a matrix of mpz_t from a text file.
 * 
 * The file format should be:
 * n m
 * a11 a12 ... a1m
 * a21 a22 ... a2m
 * ...
 * 
 * @param filename Path to the input file
 * @param n Pointer to store number of rows
 * @param m Pointer to store number of columns
 * @return Pointer to the allocated and filled mpz_t matrix
 */
mpz_t** read_mpz_matrix_from_file(const char* filename, int* n, int* m) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Error: could not open file '%s' for reading.\n", filename);
        exit(EXIT_FAILURE);
    }

    fscanf(file, "%d %d", n, m);
    mpz_t** mat = allocate_mpz_matrix(*n, *m);

    for (int i = 0; i < *n; i++) {
        for (int j = 0; j < *m; j++) {
            if (mpz_inp_str(mat[i][j], file, 10) == 0) {
                fprintf(stderr, "Error: failed to read mpz_t at (%d, %d)\n", i, j);
                exit(EXIT_FAILURE);
            }
        }
    }

    fclose(file);
    return mat;
}

/**
 * Writes a matrix of mpz_t to a text file.
 * 
 * @param filename Path to the output file
 * @param mat Pointer to the matrix
 * @param n Number of rows
 * @param m Number of columns
 */
void write_mpz_matrix_to_file(const char* filename, mpz_t** mat, int n, int m) {
    FILE* file = fopen(filename, "w");
    if (!file) {
        fprintf(stderr, "Error: could not open file '%s' for writing.\n", filename);
        exit(EXIT_FAILURE);
    }

    fprintf(file, "%d %d\n", n, m);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            mpz_out_str(file, 10, mat[i][j]);
            fprintf(file, " ");
        }
        fprintf(file, "\n");
    }

    fclose(file);
}

