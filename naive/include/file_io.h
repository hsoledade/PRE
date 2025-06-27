#ifndef FILE_IO_H
#define FILE_IO_H

/**
 * Reads a matrix of integers from a .txt file.
 * 
 * @param filename Path to the input file
 * @param n Pointer to store number of rows
 * @param m Pointer to store number of columns
 * @return Pointer to the allocated and filled matrix
 */
int** read_matrix_from_file(const char* filename, int* n, int* m);

/**
 * Writes a matrix of integers to a .txt file.
 * 
 * @param filename Path to the output file
 * @param mat Pointer to the matrix
 * @param n Number of rows
 * @param m Number of columns
 */
void write_matrix_to_file(const char* filename, int** mat, int n, int m);

#endif // FILE_IO_H
