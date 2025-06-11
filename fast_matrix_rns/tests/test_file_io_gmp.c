#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "file_io_gmp.h"
#include "matrix_utils_gmp.h"

#define TMP_FILE "results/test_file_io_gmp_output.txt"

int main() {
    int n = 2, m = 3;

    // Step 1: Create and fill a test matrix
    mpz_t** original = allocate_mpz_matrix(n, m);

    mpz_set_ui(original[0][0], 111111);
    mpz_set_ui(original[0][1], 222222);
    mpz_set_ui(original[0][2], 333333);

    mpz_set_str(original[1][0], "987654321987654321987654321", 10);
    mpz_set_str(original[1][1], "123456789123456789123456789", 10);
    mpz_set_ui(original[1][2], 999999);

    // Step 2: Write to temporary file
    write_mpz_matrix_to_file(TMP_FILE, original, n, m);

    // Step 3: Read back into another matrix
    int read_n, read_m;
    mpz_t** recovered = read_mpz_matrix_from_file(TMP_FILE, &read_n, &read_m);

    // Step 4: Verify dimensions
    assert(read_n == n);
    assert(read_m == m);

    // Step 5: Compare values element-wise
    int success = 1;
    for (int i = 0; i < n && success; i++) {
        for (int j = 0; j < m && success; j++) {
            if (mpz_cmp(original[i][j], recovered[i][j]) != 0) {
                printf("Mismatch at (%d, %d)\n", i, j);
                success = 0;
            }
        }
    }

    if (success) {
        printf("test_file_io_gmp PASSED: matrices are identical.\n");
    } else {
        printf("test_file_io_gmp FAILED: matrices differ.\n");
    }

    // Step 6: Cleanup
    free_mpz_matrix(original, n, m);
    free_mpz_matrix(recovered, read_n, read_m);

    return success ? 0 : 1;
}
