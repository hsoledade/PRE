#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "matrix_utils_int16.h"

int16_t** allocate_matrix_int16(int n, int m) {
    int16_t** mat = (int16_t**) malloc(n * sizeof(int16_t*));
    for (int i = 0; i < n; i++) {
        mat[i] = (int16_t*) malloc(m * sizeof(int16_t));
    }
    return mat;
}

void free_matrix_int16(int16_t** mat, int n) {
    for (int i = 0; i < n; i++) {
        free(mat[i]);
    }
    free(mat);
}

void print_matrix_int16(int16_t** mat, int n, int m) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            printf("%6d ", mat[i][j]);
        }
        printf("\n");
    }
}
