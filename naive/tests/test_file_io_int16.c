#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "file_io_int16.h"
#include "matrix_utils_int16.h"

int main() {
    int n = 2, m = 3;
    int16_t** mat = allocate_matrix_int16(n, m);
    mat[0][0] = 1111; mat[0][1] = -2222; mat[0][2] = 3333;
    mat[1][0] = -4444; mat[1][1] = 5555; mat[1][2] = -6666;

    save_matrix_int16("test_int16_matrix.txt", mat, n, m);

    int new_n, new_m;
    int16_t** loaded = load_matrix_int16("test_int16_matrix.txt", &new_n, &new_m);

    assert(new_n == n && new_m == m);
    assert(loaded[0][2] == 3333);
    assert(loaded[1][0] == -4444);

    printf("test_file_io_int16: passed\n");

    free_matrix_int16(mat, n);
    free_matrix_int16(loaded, new_n);
    return 0;
}
