#include <stdio.h>
#include <stdlib.h>

/**
 * Allocates a dynamic 2D integer matrix of size n × m.
 * Returns a pointer to the matrix.
 */
int** allocate_matrix(int n, int m) {
    int** mat = (int**) malloc(n * sizeof(int*));
    if (mat == NULL) {
        fprintf(stderr, "Error: failed to allocate matrix rows.\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < n; i++) {
        mat[i] = (int*) malloc(m * sizeof(int));
        if (mat[i] == NULL) {
            fprintf(stderr, "Error: failed to allocate row %d.\n", i);
            for (int k = 0; k < i; k++) free(mat[k]);
            free(mat);
            exit(EXIT_FAILURE);
        }
    }

    return mat;
}

/**
 * Frees the memory used by a dynamic 2D integer matrix.
 */
void free_matrix(int** mat, int n) {
    if (mat == NULL) return;
    for (int i = 0; i < n; i++) {
        free(mat[i]);
    }
    free(mat);
}

/**
 * Prints a 2D integer matrix to the standard output.
 */
void print_matrix(int** mat, int n, int m) {
    if (mat == NULL) {
        printf("NULL matrix.\n");
        return;
    }

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            printf("%-6d ", mat[i][j]);
        }
        printf("\n");
    }
}
