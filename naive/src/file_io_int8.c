#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "file_io_int8.h"

int8_t** load_matrix_int8(const char* filename, int* out_n, int* out_m) {
    FILE* file = fopen(filename, "r");
    if (!file) return NULL;

    fscanf(file, "%d %d", out_n, out_m);
    int8_t** mat = (int8_t**) malloc((*out_n) * sizeof(int8_t*));
    for (int i = 0; i < *out_n; i++) {
        mat[i] = (int8_t*) malloc((*out_m) * sizeof(int8_t));
        for (int j = 0; j < *out_m; j++) {
            int temp;
            fscanf(file, "%d", &temp);
            mat[i][j] = (int8_t) temp;
        }
    }
    fclose(file);
    return mat;
}

void save_matrix_int8(const char* filename, int8_t** mat, int n, int m) {
    FILE* file = fopen(filename, "w");
    if (!file) return;

    fprintf(file, "%d %d\n", n, m);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            fprintf(file, "%d ", mat[i][j]);
        }
        fprintf(file, "\n");
    }
    fclose(file);
}
