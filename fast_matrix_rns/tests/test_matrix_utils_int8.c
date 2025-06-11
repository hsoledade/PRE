#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "matrix_utils_int8.h"

int main() {
    int n = 2, m = 3;
    int8_t** mat = allocate_matrix_int8(n, m);

    mat[0][0] = 10; mat[0][1] = 20; mat[0][2] = 30;
    mat[1][0] = -40; mat[1][1] = 50; mat[1][2] = -60;

    print_matrix_int8(mat, n, m);
    assert(mat[0][2] == 30);
    assert(mat[1][0] == -40);

    free_matrix_int8(mat, n);
    printf("test_matrix_utils_int8: passed\n");
    return 0;
}
