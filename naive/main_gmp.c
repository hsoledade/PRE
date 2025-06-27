#include "file_io_gmp.h"
#include "matrix_utils_gmp.h"

int main() {
    int n, m;
    mpz_t** A = read_mpz_matrix_from_file("data/big_matrix.txt", &n, &m);
    print_mpz_matrix(A, n, m);
    write_mpz_matrix_to_file("results/copy_big_matrix.txt", A, n, m);
    free_mpz_matrix(A, n, m);
    return 0;
}
