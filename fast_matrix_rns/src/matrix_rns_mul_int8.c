#include <stdlib.h>
#include <stdint.h>
#include "matrix_rns_mul_int8.h"
#include "rns_conversion_int8.h"
#include "matrix_utils_int8.h"

// Modular inverse using extended Euclidean algorithm
int64_t modinv(int64_t a, int64_t m) {
    int64_t m0 = m, t, q;
    int64_t x0 = 0, x1 = 1;
    if (m == 1) return 0;
    while (a > 1) {
        q = a / m;
        t = m;
        m = a % m;
        a = t;
        t = x0;
        x0 = x1 - q * x0;
        x1 = t;
    }
    return (x1 + m0) % m0;
}

// Main multiplication + CRT reconstruction function
int64_t** multiply_matrix_rns_int8(int8_t** A, int8_t** B, int n, int m, int p, int* moduli, int k) {
    // Convert A and B to RNS
    RNSMatrix* Arns = int8_matrix_to_rns(A, n, m, moduli, k);
    RNSMatrix* Brns = int8_matrix_to_rns(B, m, p, moduli, k);

    // Prepare space for C residues
    int*** Cres = malloc(k * sizeof(int**));
    for (int idx = 0; idx < k; idx++) {
        Cres[idx] = malloc(n * sizeof(int*));
        for (int i = 0; i < n; i++) {
            Cres[idx][i] = calloc(p, sizeof(int));
        }
    }

    // Multiply in each modulus space
    for (int idx = 0; idx < k; idx++) {
        int mod = moduli[idx];
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < p; j++) {
                for (int r = 0; r < m; r++) {
                    Cres[idx][i][j] = (Cres[idx][i][j] + 
                        Arns->residues[idx][i][r] * Brns->residues[idx][r][j]) % mod;
                }
            }
        }
    }

    // Reconstruct with CRT
    int64_t M = 1;
    for (int i = 0; i < k; i++) M *= moduli[i];

    int64_t** C = malloc(n * sizeof(int64_t*));
    for (int i = 0; i < n; i++) {
        C[i] = malloc(p * sizeof(int64_t));
        for (int j = 0; j < p; j++) {
            int64_t x = 0;
            for (int idx = 0; idx < k; idx++) {
                int64_t mi = moduli[idx];
                int64_t ai = Cres[idx][i][j];
                int64_t Mi = M / mi;
                int64_t yi = modinv(Mi, mi);
                x += ai * Mi * yi;
            }
            C[i][j] = x % M;
        }
    }

    // Free temporary residue matrices
    for (int idx = 0; idx < k; idx++) {
        for (int i = 0; i < n; i++) {
            free(Cres[idx][i]);
        }
        free(Cres[idx]);
    }
    free(Cres);
    free_rns_matrix(Arns);
    free_rns_matrix(Brns);

    return C;
}
