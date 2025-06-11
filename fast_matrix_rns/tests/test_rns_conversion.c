#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "rns_conversion_int.h"
#include "matrix_utils.h"

int main() {
    int n = 2, m = 2;
    int** A = allocate_matrix(n, m);
    A[0][0] = 10; A[0][1] = 20;
    A[1][0] = 30; A[1][1] = 40;

    int moduli[] = {7, 11, 13};
    int k = sizeof(moduli) / sizeof(moduli[0]);

    RNSMatrix* rns = int_matrix_to_rns(A, n, m, moduli, k);

    assert(rns->residues[0][0][0] == 10 % 7);
    assert(rns->residues[1][1][1] == 40 % 11);
    assert(rns->residues[2][1][0] == 30 % 13);

    printf("All int RNS conversion tests passed.\n");

    free_matrix(A, n);
    free_rns_matrix(rns);
    return 0;
}
