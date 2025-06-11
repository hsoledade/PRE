#ifndef RNS_CONVERSION_INT_H
#define RNS_CONVERSION_INT_H

/**
 * Structure to hold a matrix represented in the Residue Number System (RNS).
 */
typedef struct {
    int*** residues;   // k matrices [n][m]
    int* moduli;       // array of k moduli
    int k;             // number of moduli
    int n, m;          // dimensions of original matrix
} RNSMatrix;

/**
 * Convert a matrix of int to its RNS representation.
 */
RNSMatrix* int_matrix_to_rns(int** A, int n, int m, int* moduli, int k);

/**
 * Free the RNSMatrix structure.
 */
void free_rns_matrix(RNSMatrix* rns);

#endif // RNS_CONVERSION_INT_H
