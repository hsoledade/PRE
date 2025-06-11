#include "matrix_utils_gmp.h"

int main() {
    int n = 2, m = 3;
    mpz_t** M = allocate_mpz_matrix(n, m);

    // Example: setting values manually
    mpz_set_ui(M[0][0], 123456789);
    mpz_set_str(M[0][1], "987654321987654321", 10); // base 10
    mpz_set_ui(M[0][2], 999);

    mpz_set_ui(M[1][0], 111);
    mpz_set_ui(M[1][1], 222);
    mpz_set_ui(M[1][2], 333);

    print_mpz_matrix(M, n, m);

    free_mpz_matrix(M, n, m);
    return 0;
}
