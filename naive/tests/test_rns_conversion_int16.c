#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include "rns_conversion_int16.h"
#include "matrix_utils_int16.h"

int main() {
    int n = 2, m = 2;
    int16_t** A = allocate_matrix_int16(n, m);
    A[0][0] = 1234; A[0][1] = -5678;
    A[1][0] = 4321; A[1][1] = 8765;

    int moduli[] = {13, 17};
    int k = 2;

    RNSMatrix* rns = int16_matrix_to_rns(A, n, m, moduli, k);

    assert(rns->residues[0][0][0] == (1234 % 13));
    assert(rns->residues[1][0][1] == ((-5678 % 17 + 17) % 17));  // resultado positivo

    printf("test_rns_conversion_int16: passed\n");

    free_matrix_int16(A, n);
    free_rns_matrix(rns);
    return 0;
}
