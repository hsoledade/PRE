#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "matrix_utils_int16.h"

int main() {
    int n = 2, m = 2;
    int16_t** mat = allocate_matrix_int16(n, m);

    mat[0][0] = 1234; mat[0][1] = -5678;
    mat[1][0] = 4321; mat[1][1] = 8765;

    print_matrix_int16(mat, n, m);
    assert(mat[0][0] == 1234);
    assert(mat[1][1] == 8765);

    free_matrix_int16(mat, n);
    printf("test_matrix_utils_int16: passed\n");
    return 0;
}
