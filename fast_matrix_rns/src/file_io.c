#include <stdio.h>
#include <stdlib.h>
#include "file_io.h"
#include "matrix_utils.h"

/**
 * Reads a matrix of integers from a .txt file.
 * 
 * The file format should be:
 * n m         ← number of rows and columns
 * a11 a12 ... a1m
 * a21 a22 ... a2m
 * ...
 * 
 * @param filename Path to the input file
 * @param n Pointer to store number of rows
 * @param m Pointer to store number of columns
 * @return Pointer to the allocated and filled matrix
 */
int** read_matrix_from_file(const char* filename, int* n, int* m) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Error: could not open file '%s' for reading.\n", filename);
        exit(EXIT_FAILURE);
    }

    fscanf(file, "%d %d", n, m);
    int** mat = allocate_matrix(*n, *m);

    for (int i = 0; i < *n; i++)
        for (int j = 0; j < *m; j++)
            fscanf(file, "%d", &mat[i][j]);

    fclose(file);
    return mat;
}

/**
 * Writes a matrix of integers to a .txt file.
 * 
 * @param filename Path to the output file
 * @param mat Pointer to the matrix
 * @param n Number of rows
 * @param m Number of columns
 */
void write_matrix_to_file(const char* filename, int** mat, int n, int m) {
    FILE* file = fopen(filename, "w");
    if (!file) {
        fprintf(stderr, "Error: could not open file '%s' for writing.\n", filename);
        exit(EXIT_FAILURE);
    }

    fprintf(file, "%d %d\n", n, m);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            fprintf(file, "%d ", mat[i][j]);
        }
        fprintf(file, "\n");
    }

    fclose(file);
}
