#ifndef RNS_CONVERSION_GMP_H
#define RNS_CONVERSION_GMP_H

#include <gmp.h>

/**
 * Structure to hold a matrix represented in the Residue Number System (RNS).
 */
typedef struct {
    int*** residues;
    int* moduli;
    int k;
    int n, m;
} RNSMatrix;

/**
 * Convert a matrix of mpz_t to its RNS representation.
 */
RNSMatrix* mpz_matrix_to_rns(mpz_t** A, int n, int m, int* moduli, int k);

/**
 * Free the RNSMatrix structure.
 */
void free_rns_matrix(RNSMatrix* rns);

#endif // RNS_CONVERSION_GMP_H
