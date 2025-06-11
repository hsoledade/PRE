#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "matrix_utils_int8.h"

int8_t** allocate_matrix_int8(int n, int m) {
    int8_t** mat = (int8_t**) malloc(n * sizeof(int8_t*));
    for (int i = 0; i < n; i++) {
        mat[i] = (int8_t*) malloc(m * sizeof(int8_t));
    }
    return mat;
}

void free_matrix_int8(int8_t** mat, int n) {
    for (int i = 0; i < n; i++) {
        free(mat[i]);
    }
    free(mat);
}

void print_matrix_int8(int8_t** mat, int n, int m) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            printf("%4d ", mat[i][j]);
        }
        printf("\n");
    }
}
