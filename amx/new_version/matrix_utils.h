#ifndef MATRIX_UTILS_H
#define MATRIX_UTILS_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <time.h>
#include "amx_matrix.h"
#include <dirent.h>

///////////////////////////////////////
////////////      All      ////////////
///////////////////////////////////////


// Function to measure time
double get_time() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec / 1000000.0;
}


///////////////////////////////////////
////////////  8uint e 8int ////////////
///////////////////////////////////////


// Print an unsigned 8-bit integer matrix
void print_matrix_u8(const uint8_t* matrix, int rows, int cols /*, const char* name*/) {
    //printf("Matrix %s (%dx%d):\n", name, rows, cols);
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
void print_matrix_i8(const int8_t* matrix, int rows, int cols /*, const char* name*/) {
    //printf("Matrix %s (%dx%d):\n", name, rows, cols);
    for (int i = 0; i < rows; i++) {
        printf("  [");
        for (int j = 0; j < cols; j++) {
            printf("%3d", matrix[i * cols + j]);
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
            fprintf(file, "%3d", matrix[i * cols + j]);
            if (j < cols - 1) fprintf(file, " ");
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
            fprintf(file, "%3u", matrix[i * cols + j]);
            if (j < cols - 1) fprintf(file, " ");
        }
        fprintf(file, "\n");
    }

    fclose(file);
    return 0;
}

/**
 * Generate 10 random int8_t matrices of size (rows × cols)
 * and save them in: matrices/int8/<rows>x<cols>/matrix_#.ssv
 */
void generate_random_i8_matrices(int rows, int cols) {
    const int count = 10;
    char folder_path[256];

    // Build path: matrices/int8/<rows>x<cols>
    snprintf(folder_path, sizeof(folder_path), "matrices/int8/%dx%d", rows, cols);

    // Create intermediate folders if they don't exist
    struct stat st = {0};
    if (stat("matrices", &st) == -1) mkdir("matrices", 0700);
    if (stat("matrices/int8", &st) == -1) mkdir("matrices/int8", 0700);
    if (stat(folder_path, &st) == -1) mkdir(folder_path, 0700);

    // Seed RNG
    srand(time(NULL));

    for (int i = 0; i < count; i++) {
        int8_t* matrix = malloc(rows * cols * sizeof(int8_t));
        if (!matrix) {
            fprintf(stderr, "Memory allocation failed for matrix %d\n", i);
            continue;
        }

        // Fill matrix with random values in [-64, 63]
        for (int j = 0; j < rows * cols; j++) {
            matrix[j] = (rand() % 128) - 64;
        }

        // Build filename: matrices/int8/<rows>x<cols>/matrix_i.ssv
        char filename[300];
        snprintf(filename, sizeof(filename), "%s/matrix_%d.ssv", folder_path, i);

        // Save to file
        if (save_matrix_i8_to_file(matrix, rows, cols, filename) == 0) {
            printf("Saved matrix %d to %s\n", i, filename);
        } else {
            fprintf(stderr, "Failed to save matrix %d\n", i);
        }

        free(matrix);
    }

    printf("Done generating 10 random int8_t matrices of size %dx%d.\n", rows, cols);
}

/**
 * Generate 10 random uint8_t matrices of size (rows × cols)
 * and save them in: matrices/uint8/<rows>x<cols>/matrix_#.ssv
 */
void generate_random_u8_matrices(int rows, int cols) {
    const int count = 10;
    char folder_path[256];

    // Build path: matrices/uint8/<rows>x<cols>
    snprintf(folder_path, sizeof(folder_path), "matrices/uint8/%dx%d", rows, cols);

    // Create intermediate folders if they don't exist
    struct stat st = {0};
    if (stat("matrices", &st) == -1) mkdir("matrices", 0700);
    if (stat("matrices/uint8", &st) == -1) mkdir("matrices/uint8", 0700);
    if (stat(folder_path, &st) == -1) mkdir(folder_path, 0700);

    // Seed RNG
    srand(time(NULL));

    for (int i = 0; i < count; i++) {
        uint8_t* matrix = malloc(rows * cols * sizeof(uint8_t));
        if (!matrix) {
            fprintf(stderr, "Memory allocation failed for matrix %d\n", i);
            continue;
        }

        // Fill matrix with random values in [0, 255]
        for (int j = 0; j < rows * cols; j++) {
            matrix[j] = rand() % 256;
        }

        // Build filename: matrices/int8/<rows>x<cols>/matrix_i.ssv
        char filename[300];
        snprintf(filename, sizeof(filename), "%s/matrix_%d.ssv", folder_path, i);

        // Save to file
        if (save_matrix_u8_to_file(matrix, rows, cols, filename) == 0) {
            printf("Saved matrix %d to %s\n", i, filename);
        } else {
            fprintf(stderr, "Failed to save matrix %d\n", i);
        }

        free(matrix);
    }

    printf("Done generating 10 random unt8_t matrices of size %dx%d.\n", rows, cols);
}

// Load an int8_t matrix from a .ssv or .txt file
int8_t* load_matrix_i8_from_file(const char* filename, int* rows, int* cols) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open matrix file");
        return NULL;
    }

    // Read dimensions
    if (fscanf(file, "%d %d", rows, cols) != 2) {
        fprintf(stderr, "Invalid matrix dimensions in file\n");
        fclose(file);
        return NULL;
    }

    int total = (*rows) * (*cols);
    int8_t* matrix = malloc(total * sizeof(int8_t));
    if (!matrix) {
        fprintf(stderr, "Memory allocation failed\n");
        fclose(file);
        return NULL;
    }

    // Read matrix values line by line
    for (int i = 0; i < *rows; i++) {
        for (int j = 0; j < *cols; j++) {
            int val;
            if (fscanf(file, "%d", &val) != 1) {
                fprintf(stderr, "Invalid or missing matrix value at (%d,%d)\n", i, j);
                free(matrix);
                fclose(file);
                return NULL;
            }
            matrix[i * (*cols) + j] = (int8_t)val;
        }
    }

    fclose(file);
    return matrix;
}

// Load an uint8_t matrix from a .ssv or .txt file
uint8_t* load_matrix_u8_from_file(const char* filename, int* rows, int* cols) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open matrix file");
        return NULL;
    }

    // Read dimensions
    if (fscanf(file, "%d %d", rows, cols) != 2) {
        fprintf(stderr, "Invalid matrix dimensions in file\n");
        fclose(file);
        return NULL;
    }

    int total = (*rows) * (*cols);
    uint8_t* matrix = malloc(total * sizeof(uint8_t));
    if (!matrix) {
        fprintf(stderr, "Memory allocation failed\n");
        fclose(file);
        return NULL;
    }

    // Read matrix values line by line
    for (int i = 0; i < *rows; i++) {
        for (int j = 0; j < *cols; j++) {
            uint val;
            if (fscanf(file, "%u", &val) != 1) {
                fprintf(stderr, "Invalid or missing matrix value at (%d,%d)\n", i, j);
                free(matrix);
                fclose(file);
                return NULL;
            }
            matrix[i * (*cols) + j] = (uint8_t)val;
        }
    }

    fclose(file);
    return matrix;
}

// Benchmark multiple matrix pairs from 0 to number in the folders matrices/int8/MxK and matrices/uint8/KxN
void benchmark_uint8_int8_matmul_one_pair(int M, int K, int N) {
    int number = 10;  // Número de matrizes para testar (0 a 9)
    
    // Cria pasta results/ se não existir
    struct stat st = {0};
    if (stat("results", &st) == -1) {
        mkdir("results", 0700);
    }
    
    // Abre o arquivo de resultados UMA VEZ
    char result_path[256];
    snprintf(result_path, sizeof(result_path), "results/times_%dx%dx%d.ssv", M, K, N);
    
    FILE* file = fopen(result_path, "w");
    if (!file) {
        perror("Failed to create time file");
        return;
    }
    
    // Escreve cabeçalho
    //fprintf(file, "%d %d %d\n", M, K, N);
    
    printf("Benchmarking %dx%dx%d matrices (0 to %d)...\n", M, K, N, number-1);
    
    int successful = 0;
    int failed = 0;
    
    // Loop de 0 até number-1
    for (int i = 0; i < number; i++) {
        char path_A[256], path_B[256];
        snprintf(path_A, sizeof(path_A), "matrices/uint8/%dx%d/matrix_%d.ssv", M, K, i);
        snprintf(path_B, sizeof(path_B), "matrices/int8/%dx%d/matrix_%d.ssv", K, N, i);
        
        printf("Testing matrix_%d... ", i);
        
        // Carrega as matrizes
        int rowsA, colsA, rowsB, colsB;
        uint8_t* A = load_matrix_u8_from_file(path_A, &rowsA, &colsA);
        int8_t* B = load_matrix_i8_from_file(path_B, &rowsB, &colsB);
        
        if (!A || !B) {
            fprintf(stderr, "Failed to load matrix_%d\n", i);
            if (A) free(A);
            if (B) free(B);
            failed++;
            continue;
        }
        
        // Verifica dimensões carregadas
        if (rowsA != M || colsA != K || rowsB != K || colsB != N) {
            fprintf(stderr, "Dimension mismatch in matrix_%d! A=%dx%d, B=%dx%d, expected A=%dx%d, B=%dx%d\n", 
                    i, rowsA, colsA, rowsB, colsB, M, K, K, N);
            free(A); 
            free(B);
            failed++;
            continue;
        }
        
        int32_t* C = calloc(M * N, sizeof(int32_t));
        if (!C) {
            fprintf(stderr, "Failed to allocate result matrix for matrix_%d\n", i);
            free(A); 
            free(B);
            failed++;
            continue;
        }
        
        // Measure time
        double start = get_time();
        int result = amx_multiply_int8_to_int32(A, B, C, M, K, N);
        double end = get_time();
        
        if (result != 0) {
            fprintf(stderr, "Matrix multiplication failed for matrix_%d (code %d)\n", i, result);
            free(A); 
            free(B); 
            free(C);
            failed++;
            continue;
        }
        
        // Salva o tempo no arquivo (em milissegundos)
        double time_ms = (end - start) * 1000;
        fprintf(file, "%.8f\n", time_ms);
        
        printf("%.8f ms\n", time_ms);
        
        free(A); 
        free(B); 
        free(C);
        successful++;
    }
    
    // Fecha o arquivo
    fclose(file);
    
    printf("\nBenchmark completed!\n");
    printf("Successful: %d/%d\n", successful, number);
    printf("Failed: %d/%d\n", failed, number);
    printf("Results saved in: %s\n", result_path);
    
    if (successful > 0) {
        printf("Average performance: Ready for analysis!\n");
    } else {
        fprintf(stderr, "No successful tests - check your matrix files!\n");
    }
}

///////////////////////////////////////
////////////  16uint e 16int //////////
///////////////////////////////////////

// TO REPAIR :
// TO REPAIR :
// TO REPAIR :

/*

// Print an unsigned 16-bit integer matrix
void print_matrix_u16(const uint16_t* matrix, int rows, int cols) {
    //printf("Matrix %s (%dx%d):\n", name, rows, cols);
    for (int i = 0; i < rows; i++) {
        printf("  [");
        for (int j = 0; j < cols; j++) {
            printf("%3u", matrix[i * cols + j]);
            if (j < cols - 1) printf(" ");
        }
        printf("]\n");
    }
}

// Print a signed 16-bit integer matrix
void print_matrix_i16(const int16_t* matrix, int rows, int cols ) {
    //printf("Matrix %s (%dx%d):\n", name, rows, cols);
    for (int i = 0; i < rows; i++) {
        printf("  [");
        for (int j = 0; j < cols; j++) {
            printf("%3d", matrix[i * cols + j]);
            if (j < cols - 1) printf(" ");
        }
        printf("]\n");
    }
}

// Fill a uint8_t matrix with a pattern
void fill_matrix_u16(uint16_t* matrix, int rows, int cols, int16_t pattern) {
    for (int i = 0; i < rows * cols; i++) {
        matrix[i] = pattern + (i % 65535);  // Evita overflow
    }
}

// Fill a int8_t matrix with a pattern
void fill_matrix_i16(int16_t* matrix, int rows, int cols, int16_t pattern) {
    for (int i = 0; i < rows * cols; i++) {
        matrix[i] = pattern + (i % 4000);  // Evita overflow
    }
}

*/


///////////////////////////////////////
////////////     32int     ////////////
///////////////////////////////////////


// Print a 32-bit integer matrix
void print_matrix_i32(const int32_t* matrix, int rows, int cols /*, const char* name*/) {
    //printf("Matrix %s (%dx%d):\n", name, rows, cols);
    for (int i = 0; i < rows; i++) {
        printf("  [");
        for (int j = 0; j < cols; j++) {
            printf("%6d", matrix[i * cols + j]);
            if (j < cols - 1) printf(" ");
        }
        printf("]\n");
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
            if (j < cols - 1) fprintf(file, " ");
        }
        fprintf(file, "\n");
    }

    fclose(file);
    return 0;
}



#endif // MATRIX_UTILS_H
