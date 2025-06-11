#include <stdlib.h>
#include <gmp.h>
#include "rns_conversion_gmp.h"

RNSMatrix* mpz_matrix_to_rns(mpz_t** A, int n, int m, int* moduli, int k) {
    RNSMatrix* rns = malloc(sizeof(RNSMatrix));
    rns->k = k;
    rns->n = n;
    rns->m = m;
    rns->moduli = malloc(k * sizeof(int));

    for (int i = 0; i < k; i++) {
        rns->moduli[i] = moduli[i];
    }

    rns->residues = malloc(k * sizeof(int**));
    for (int mod_idx = 0; mod_idx < k; mod_idx++) {
        int** mat = malloc(n * sizeof(int*));
        for (int i = 0; i < n; i++) {
            mat[i] = malloc(m * sizeof(int));
            for (int j = 0; j < m; j++) {
                mat[i][j] = mpz_fdiv_ui(A[i][j], moduli[mod_idx]);
            }
        }
        rns->residues[mod_idx] = mat;
    }

    return rns;
}

void free_rns_matrix(RNSMatrix* rns) {
    if (!rns) return;
    for (int mod_idx = 0; mod_idx < rns->k; mod_idx++) {
        for (int i = 0; i < rns->n; i++) {
            free(rns->residues[mod_idx][i]);
        }
        free(rns->residues[mod_idx]);
    }
    free(rns->residues);
    free(rns->moduli);
    free(rns);
}
