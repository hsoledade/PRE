#include <stdio.h>
#include <stdlib.h>
#include <gmp.h>
#include <assert.h>
#include "rns_conversion_gmp.h"
#include "matrix_utils_gmp.h"

int main() {
    int n = 2, m = 2;
    mpz_t** A = allocate_mpz_matrix(n, m);

    mpz_set_ui(A[0][0], 100);
    mpz_set_ui(A[0][1], 200);
    mpz_set_ui(A[1][0], 300);
    mpz_set_ui(A[1][1], 400);

    int moduli[] = {7, 11};
    int k = sizeof(moduli) / sizeof(moduli[0]);

    RNSMatrix* rns = mpz_matrix_to_rns(A, n, m, moduli, k);

    // Comparação usando valores diretos
    assert(rns->residues[0][0][0] == mpz_fdiv_ui(A[0][0], 7));
    assert(rns->residues[1][1][1] == mpz_fdiv_ui(A[1][1], 11));

    printf("All GMP RNS conversion tests passed.\n");

    free_mpz_matrix(A, n, m);
    free_rns_matrix(rns);
    return 0;
}
