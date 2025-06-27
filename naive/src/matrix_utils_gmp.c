#include <stdio.h>
#include <stdlib.h>
#include <gmp.h>

/**
 * Allocates a 2D matrix of mpz_t (multi-precision integers) of size n × m.
 * Initializes each element with mpz_init.
 */
mpz_t** allocate_mpz_matrix(int n, int m) {
    mpz_t** mat = (mpz_t**) malloc(n * sizeof(mpz_t*));
    if (mat == NULL) {
        fprintf(stderr, "Error: failed to allocate mpz_t matrix rows.\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < n; i++) {
        mat[i] = (mpz_t*) malloc(m * sizeof(mpz_t));
        if (mat[i] == NULL) {
            fprintf(stderr, "Error: failed to allocate row %d for mpz_t.\n", i);
            for (int k = 0; k < i; k++) free(mat[k]);
            free(mat);
            exit(EXIT_FAILURE);
        }
        for (int j = 0; j < m; j++) {
            mpz_init(mat[i][j]);
        }
    }

    return mat;
}

/**
 * Frees a 2D mpz_t matrix (calls mpz_clear on each element).
 */
void free_mpz_matrix(mpz_t** mat, int n, int m) {
    if (mat == NULL) return;

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            mpz_clear(mat[i][j]);
        }
        free(mat[i]);
    }
    free(mat);
}

/**
 * Prints a 2D mpz_t matrix to the standard output.
 */
void print_mpz_matrix(mpz_t** mat, int n, int m) {
    if (mat == NULL) {
        printf("NULL mpz_t matrix.\n");
        return;
    }

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            gmp_printf("%Zd ", mat[i][j]);
        }
        printf("\n");
    }
}
