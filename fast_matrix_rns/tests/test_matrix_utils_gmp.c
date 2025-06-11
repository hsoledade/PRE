#include <stdio.h>
#include <assert.h>
#include "matrix_utils_gmp.h"

int main() {
    int n = 2, m = 2;
    mpz_t** mat = allocate_mpz_matrix(n, m);

    // Fill values
    mpz_set_ui(mat[0][0], 123);
    mpz_set_ui(mat[0][1], 456);
    mpz_set_str(mat[1][0], "987654321987654321", 10);
    mpz_set_ui(mat[1][1], 789);

    // Assertions
    assert(mpz_cmp_ui(mat[0][0], 123) == 0);
    assert(mpz_cmp_ui(mat[0][1], 456) == 0);
    assert(mpz_cmp_ui(mat[1][1], 789) == 0);

    // Compare mat[1][0] with expected big integer "987654321987654321"
    mpz_t expected;
    mpz_init_set_str(expected, "987654321987654321", 10);
    assert(mpz_cmp(mat[1][0], expected) == 0);
    mpz_clear(expected);

    // Visual check
    printf("test_matrix_utils_gmp: matrix content:\n");
    print_mpz_matrix(mat, n, m);

    // Free memory
    free_mpz_matrix(mat, n, m);
    return 0;
}
