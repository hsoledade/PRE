#ifndef MATRIX_RNS_MUL_INT8_H
#define MATRIX_RNS_MUL_INT8_H

#include <stdint.h>

/**
 * Multiply two int8_t matrices A[n][m] and B[m][p] using RNS.
 * Resulting matrix is reconstructed using the Chinese Remainder Theorem.
 * 
 * @param A First input matrix (int8_t)
 * @param B Second input matrix (int8_t)
 * @param n Number of rows in A
 * @param m Number of columns in A and rows in B
 * @param p Number of columns in B
 * @param moduli Array of k pairwise coprime integers
 * @param k Number of moduli
 * @return Matrix of size n × p with int64_t entries reconstructed from RNS
 */
int64_t** multiply_matrix_rns_int8(int8_t** A, int8_t** B, int n, int m, int p, int* moduli, int k);

#endif // MATRIX_RNS_MUL_INT8_H
