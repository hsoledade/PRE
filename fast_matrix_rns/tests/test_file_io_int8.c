#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "file_io_int8.h"
#include "matrix_utils_int8.h"

int main() {
    int n = 2, m = 2;
    int8_t** mat = allocate_matrix_int8(n, m);
    mat[0][0] = 11; mat[0][1] = -22;
    mat[1][0] = 33; mat[1][1] = -44;

    save_matrix_int8("test_int8_matrix.txt", mat, n, m);

    int new_n, new_m;
    int8_t** loaded = load_matrix_int8("test_int8_matrix.txt", &new_n, &new_m);

    assert(new_n == n && new_m == m);
    assert(loaded[0][0] == 11);
    assert(loaded[1][1] == -44);

    printf("test_file_io_int8: passed\n");

    free_matrix_int8(mat, n);
    free_matrix_int8(loaded, new_n);
    return 0;
}
