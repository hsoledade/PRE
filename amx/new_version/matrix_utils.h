#ifndef MATRIX_UTILS_H
#define MATRIX_UTILS_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// Print an unsigned 8-bit integer matrix
void print_matrix_u8(const uint8_t* matrix, int rows, int cols, const char* name) {
    printf("Matrix %s (%dx%d):\n", name, rows, cols);
    for (int i = 0; i < rows; i++) {
        printf("  [");
        for (int j = 0; j < cols; j++) {
            printf("%3u", matrix[i * cols + j]);
            if (j < cols - 1) printf(" ");
        }
        printf("]\n");
    }
}

// Print a signed 8-bit integer matrix
void print_matrix_i8(const int8_t* matrix, int rows, int cols, const char* name) {
    printf("Matrix %s (%dx%d):\n", name, rows, cols);
    for (int i = 0; i < rows; i++) {
        printf("  [");
        for (int j = 0; j < cols; j++) {
            printf("%4d", matrix[i * cols + j]);
            if (j < cols - 1) printf(" ");
        }
        printf("]\n");
    }
}

// Print a 32-bit integer matrix
void print_matrix_i32(const int32_t* matrix, int rows, int cols, const char* name) {
    printf("Matrix %s (%dx%d):\n", name, rows, cols);
    for (int i = 0; i < rows; i++) {
        printf("  [");
        for (int j = 0; j < cols; j++) {
            printf("%6d", matrix[i * cols + j]);
            if (j < cols - 1) printf(" ");
        }
        printf("]\n");
    }
}

// Fill a uint8_t matrix with a pattern
void fill_matrix_u8(uint8_t* matrix, int rows, int cols, uint8_t pattern) {
    for (int i = 0; i < rows * cols; i++) {
        matrix[i] = (pattern + i) % 256;
        //matrix[i] = 1;
    }
}

// Fill an int8_t matrix with a pattern
void fill_matrix_i8(int8_t* matrix, int rows, int cols, int8_t pattern) {
    for (int i = 0; i < rows * cols; i++) {
        matrix[i] = (pattern + i) % 128 - 64;  // Range -64 to 63
        //matrix[i] = 1;
    }
}

// Checks if there are any unexpected zeros in the result matrix, and prints the sum, minimum, and maximum values for inspection.
void verify_result(const int32_t* C, int M, int N, const char* test_name) {
    int zero_count = 0;
    int32_t sum = 0;
    int32_t min_val = C[0], max_val = C[0];
    
    for (int i = 0; i < M*N; i++) {
        //printf("%3d ", C[i]);
        if (C[i] == 0) zero_count++;
        sum += C[i];
        if (C[i] < min_val) min_val = C[i];
        if (C[i] > max_val) max_val = C[i];
    }
    printf("\n  %s - Sum: %ld, Min: %d, Max: %d, Zeros: %d/%d\n", test_name, (long)sum, min_val, max_val, zero_count, M*N);
}

// Save an int8_t matrix to a .ssv or .txt file.
int save_matrix_i8_to_file(const int8_t* matrix, int rows, int cols, const char* filename) {
    FILE* file = fopen(filename, "w");
    if (!file) {
        perror("Failed to open file for writing");
        return -1;
    }

    // Write dimensions
    fprintf(file, "%d %d\n", rows, cols);

    // Write matrix values row by row
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            fprintf(file, "%d", matrix[i * cols + j]);
            if (j < cols - 1) fprintf(file, ";");
        }
        fprintf(file, "\n");
    }

    fclose(file);
    return 0;
}

// Save an uint8_t matrix to a .ssv or .txt file.
int save_matrix_u8_to_file(const uint8_t* matrix, int rows, int cols, const char* filename) {
    FILE* file = fopen(filename, "w");
    if (!file) {
        perror("Failed to open file for writing");
        return -1;
    }

    // Write dimensions
    fprintf(file, "%d %d\n", rows, cols);

    // Write matrix values row by row
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            fprintf(file, "%d", matrix[i * cols + j]);
            if (j < cols - 1) fprintf(file, ";");
        }
        fprintf(file, "\n");
    }

    fclose(file);
    return 0;
}

// Save an int32_t matrix to a .ssv or .txt file.
int save_matrix_i32_to_file(const int32_t* matrix, int rows, int cols, const char* filename) {
    FILE* file = fopen(filename, "w");
    if (!file) {
        perror("Failed to open file for writing");
        return -1;
    }

    // Write dimensions
    fprintf(file, "%d %d\n", rows, cols);

    // Write matrix values row by row
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            fprintf(file, "%d", matrix[i * cols + j]);
            if (j < cols - 1) fprintf(file, ";");
        }
        fprintf(file, "\n");
    }

    fclose(file);
    return 0;
}

#endif // MATRIX_UTILS_H
