#ifndef RNS_CONVERSION_INT16_H
#define RNS_CONVERSION_INT16_H

#include <stdint.h>

typedef struct {
    int*** residues;  // k matrices of size n × m, one for each modulus (residues[i][row][col])
    int* moduli;      // array of k moduli: [m1, m2, ..., mk]
    int k;            // number of moduli
    int n, m;         // dimensions of the original matrix
} RNSMatrix;

/**
 * Convert an int16_t matrix to RNSMatrix.
 */
RNSMatrix* int16_matrix_to_rns(int16_t** A, int n, int m, int* moduli, int k);

/**
 * Free the RNSMatrix.
 */
void free_rns_matrix(RNSMatrix* rns);

#endif // RNS_CONVERSION_INT16_H
