#include <stdio.h>
#include <assert.h>
#include "matrix_utils.h"

int main() {
    int n = 2, m = 2;
    int** mat = allocate_matrix(n, m);

    // Fill values
    mat[0][0] = 10;
    mat[0][1] = 20;
    mat[1][0] = 30;
    mat[1][1] = 40;

    // Assertions
    assert(mat[0][0] == 10);
    assert(mat[0][1] == 20);
    assert(mat[1][0] == 30);
    assert(mat[1][1] == 40);

    // Visual check
    printf("test_matrix_utils: matrix content:\n");
    print_matrix(mat, n, m);

    // Free memory
    free_matrix(mat, n);

    return 0;
}
