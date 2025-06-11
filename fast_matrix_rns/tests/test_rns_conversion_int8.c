#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include "rns_conversion_int8.h"
#include "matrix_utils_int8.h"

int main() {
    int n = 2, m = 2;
    int8_t** A = allocate_matrix_int8(n, m);
    A[0][0] = 12; A[0][1] = -9;
    A[1][0] = 7;  A[1][1] = 15;

    int moduli[] = {5, 11};
    int k = 2;

    RNSMatrix* rns = int8_matrix_to_rns(A, n, m, moduli, k);

    assert(rns->residues[0][0][0] == (12 % 5));
    assert(rns->residues[1][0][1] == ((-9 % 11 + 11) % 11));  // garantir resultado positivo

    printf("test_rns_conversion_int8: passed\n");

    free_matrix_int8(A, n);
    free_rns_matrix(rns);
    return 0;
}
