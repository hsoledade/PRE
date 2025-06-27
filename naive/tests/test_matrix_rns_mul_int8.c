#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include "matrix_utils_int8.h"
#include "matrix_rns_mul_int8.h"

int main() {
    int n = 2, m = 3, p = 2;
    int8_t** A = allocate_matrix_int8(n, m);
    int8_t** B = allocate_matrix_int8(m, p);

    // A = [1 2 3; 4 5 6]
    A[0][0] = 1; A[0][1] = 2; A[0][2] = 3;
    A[1][0] = 4; A[1][1] = 5; A[1][2] = 6;

    // B = [7 8; 9 10; 11 12]
    B[0][0] = 7;  B[0][1] = 8;
    B[1][0] = 9;  B[1][1] = 10;
    B[2][0] = 11; B[2][1] = 12;

    int moduli[] = {257, 263, 269}; // Pairwise coprime
    int k = 3;

    int64_t** C = multiply_matrix_rns_int8(A, B, n, m, p, moduli, k);

    // Expected result: C = A * B = [58 64; 139 154]
    assert(C[0][0] == 58);
    assert(C[0][1] == 64);
    assert(C[1][0] == 139);
    assert(C[1][1] == 154);

    printf("test_matrix_rns_mul_int8: passed\n");

    // Free memory
    free_matrix_int8(A, n);
    free_matrix_int8(B, m);
    for (int i = 0; i < n; i++) free(C[i]);
    free(C);

    return 0;
}
