#ifndef MATRIX_UTILS_H
#define MATRIX_UTILS_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
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
        int result = amx_multiply_uint8_int8_to_int32(A, B, C, M, K, N);
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

void test_uint8_int8_completo() {
    printf("===================================\n");
    printf("=== TESTE UINT8 X INT8 COMPLETO ===\n");
    printf("===================================\n");
    
    /////////////////////////////////////
    //////////////  Teste 1  ////////////
    /////////////////////////////////////

    uint8_t A1[] = {2, 3};
    int8_t B1[] = {1, 4};
    int32_t C1[1] = {0};
    printf("Teste 1: A=[2,3], B=[1,4] (esperado: 2*1 + 3*4 = 14)\n");
    amx_multiply_small_uint8_int8_to_int32(A1, B1, C1, 1, 2, 1);
    printf("Resultado: %d - %s\n\n", C1[0], (C1[0] == 14) ? "✓ CORRETO" : "✗ INCORRETO");
    
    /////////////////////////////////////
    // Teste 2: Matriz 2x2 × vetor 2x1 //
    /////////////////////////////////////

    uint8_t A2[] = {1, 2, 3, 4}; // [[1,2], [3,4]]
    int8_t B2[] = {5, 6}; // [[5], [6]]
    int32_t C2[2] = {0};
    printf("Teste 2: A=[[1,2],[3,4]], B=[[5],[6]] (esperado: [17, 39])\n");
    printf("Cálculo: [1*5+2*6, 3*5+4*6] = [5+12, 15+24] = [17, 39]\n");
    amx_multiply_small_uint8_int8_to_int32(A2, B2, C2, 2, 2, 1);
    printf("Resultado: [%d, %d] - %s\n\n", C2[0], C2[1],
           (C2[0]==17 && C2[1]==39) ? "✓ CORRETO" : "✗ INCORRETO");
    
    /////////////////////////////////////////////////////////////       
    // Teste 3: Matriz 2x3 × matriz 3x2 (como no teste 16-bit) //
    /////////////////////////////////////////////////////////////

    uint8_t A3[] = {1, 2, 3, 4, 5, 6}; // [[1,2,3], [4,5,6]]
    int8_t B3[] = {1, 2, 3, 4, 5, 6}; // [[1,2], [3,4], [5,6]]
    int32_t C3[4] = {0};
    printf("Teste 3: A=[[1,2,3],[4,5,6]], B=[[1,2],[3,4],[5,6]]\n");
    printf("Esperado: C[0][0]=22, C[0][1]=28, C[1][0]=49, C[1][1]=64\n");
    printf("Cálculo C[0][0]: 1*1 + 2*3 + 3*5 = 1 + 6 + 15 = 22\n");
    printf("Cálculo C[0][1]: 1*2 + 2*4 + 3*6 = 2 + 8 + 18 = 28\n");
    amx_multiply_small_uint8_int8_to_int32(A3, B3, C3, 2, 3, 2);
    printf("Resultado: C[0][0]=%d, C[0][1]=%d, C[1][0]=%d, C[1][1]=%d\n", C3[0], C3[1], C3[2], C3[3]);
    int corretos = (C3[0]==22) + (C3[1]==28) + (C3[2]==49) + (C3[3]==64);
    printf("Status: %d/4 corretos - %s\n\n", corretos, (corretos==4) ? "✓ CORRETO" : "✗ INCORRETO");
    
    //////////////////////////////////////////////////////////////////
    // Teste 4: Valores que usam byte alto (>255 quando combinados) //
    //////////////////////////////////////////////////////////////////

    uint8_t A4[] = {100, 200}; // FAZ SENTIDO DAR ERRADO, PQ O Int8 so vai até 127
    int8_t B4[] = {3, 2};
    int32_t C4[1] = {0};
    printf("Teste 4: A=[100,200], B=[3,2] (esperado: 100*3 + 200*2 = 700)\n");
    amx_multiply_small_uint8_int8_to_int32(A4, B4, C4, 1, 2, 1);
    printf("Resultado: %d - %s\n\n", C4[0], (C4[0] == 700) ? "✓ CORRETO" : "✗ INCORRETO");
    
    ////////////////////////////////////////////////////////////
    // Teste 5: Forçar uso da função large (M=20, K=100, N=5) //
    ////////////////////////////////////////////////////////////
    printf("=== TESTES DIMENSÕES GRANDES ===\n");
    printf("Teste 5: Matriz Grande 20x100 x 100x5 (usa função large)\n");
    uint8_t* A5 = malloc(20 * 100 * sizeof(uint8_t));
    int8_t* B5 = malloc(100 * 5 * sizeof(int8_t));
    int32_t* C5 = malloc(20 * 5 * sizeof(int32_t));
    
    // Preencher com padrão conhecido
    for (int i = 0; i < 20; i++) {
        for (int k = 0; k < 100; k++) {
            A5[i * 100 + k] = 2;
        }
    }

    for (int k = 0; k < 100; k++) {
        for (int j = 0; j < 5; j++) {
            B5[k * 5 + j] = 1; 
        }
    }
    
    int result5 = amx_multiply_uint8_int8_to_int32(A5, B5, C5, 20, 100, 5);
    
    printf("Status: %s\n", (result5 == AMX_SUCCESS) ? "✓ SUCESSO" : "✗ ERRO");
    printf("Resultado: %d - %s\n\n", C5[0], (C5[0] == 200) ? "✓ CORRETO" : "✗ INCORRETO");
    
    free(A5); free(B5); free(C5);
    
    // Teste 6: Dimensões que forçam múltiplos blocos (M=32, K=128, N=16)
    printf("\nTeste 6: Múltiplos Blocos 32x128 × 128x16\n");
    uint8_t* A6 = malloc(32 * 128 * sizeof(uint8_t));
    int8_t* B6 = malloc(128 * 16 * sizeof(int8_t));
    int32_t* C6 = malloc(32 * 16 * sizeof(int32_t));
    
    // Padrão simples para verificação
    for (int i = 0; i < 32 * 128; i++) A6[i] = (i % 200) + 1;
    for (int i = 0; i < 128 * 16; i++) B6[i] = (i % 100) + 1;
    
    int result6 = amx_multiply_uint8_int8_to_int32(A6, B6, C6, 32, 128, 16);
    
    printf("Status: %s\n", (result6 == AMX_SUCCESS) ? "✓ SUCESSO" : "✗ ERRO");
    printf("Amostra C[0][0]=%d, C[31][15]=%d\n", C6[0], C6[31*16+15]);
    
    free(A6); free(B6); free(C6);
    
    // Teste 7: K muito grande (M=8, K=512, N=4)
    printf("\nTeste 7: K Muito Grande 8x512 × 512x4\n");
    uint8_t* A7 = malloc(8 * 512 * sizeof(uint8_t));
    int8_t* B7 = malloc(512 * 4 * sizeof(int8_t));
    int32_t* C7 = malloc(8 * 4 * sizeof(int32_t));
    
    for (int i = 0; i < 8 * 512; i++) A7[i] = 1; // Todos 1 para fácil verificação
    for (int i = 0; i < 512 * 4; i++) B7[i] = 2; // Todos 2
    // Resultado esperado: cada C[i][j] = 512 * 1 * 2 = 1024
    
    int result7 = amx_multiply_uint8_int8_to_int32(A7, B7, C7, 8, 512, 4);
    
    printf("Status: %s\n", (result7 == AMX_SUCCESS) ? "✓ SUCESSO" : "✗ ERRO");
    printf("Esperado: todos 1024, Obtido: C[0][0]=%d, C[7][3]=%d\n", C7[0], C7[7*4+3]);
    bool teste7_ok = (C7[0] == 1024) && (C7[7*4+3] == 1024);
    printf("Verificação: %s\n", teste7_ok ? "✓ VALORES CORRETOS" : "✗ VALORES INCORRETOS");
    
    free(A7); free(B7); free(C7);
    
    // Teste 8: Verificação vs CPU simples (M=64, K=64, N=64)
    printf("\nTeste 8: Verificação AMX vs CPU 64x64 × 64x64\n");
    uint8_t* A8 = malloc(64 * 64 * sizeof(uint8_t));
    int8_t* B8 = malloc(64 * 64 * sizeof(int8_t));
    int32_t* C8_amx = malloc(64 * 64 * sizeof(int32_t));
    int32_t* C8_cpu = malloc(64 * 64 * sizeof(int32_t));
    
    // Dados aleatórios pequenos
    for (int i = 0; i < 64 * 64; i++) {
        A8[i] = (i % 10) + 1;
        B8[i] = (i % 5) + 1;
    }
    
    // Teste AMX
    amx_multiply_uint8_int8_to_int32(A8, B8, C8_amx, 64, 64, 64);
    
    // Teste CPU simples
    memset(C8_cpu, 0, 64 * 64 * sizeof(int32_t));
    for (int i = 0; i < 64; i++) {
        for (int j = 0; j < 64; j++) {
            for (int k = 0; k < 64; k++) {
                C8_cpu[i * 64 + j] += A8[i * 64 + k] * B8[k * 64 + j];
            }
        }
    }
    
    // Verificar se resultados são iguais
    bool results_match = true;
    for (int i = 0; i < 64 * 64; i++) {
        if (C8_amx[i] != C8_cpu[i]) {
            results_match = false;
            break;
        }
    }
    
    printf("Resultados AMX vs CPU: %s\n", results_match ? "✓ IGUAIS" : "✗ DIFERENTES");
    if (!results_match) {
        printf("Primeira diferença: AMX=%d, CPU=%d\n", C8_amx[0], C8_cpu[0]);
    }
    
    free(A8); free(B8); free(C8_amx); free(C8_cpu);
    
    printf("\n=== RESUMO TESTE INT8 ===\n");
    int total_corretos = (C1[0]==14) + (C2[0]==17 && C2[1]==39) + (corretos==4) + (C4[0]==700);
    printf("Testes básicos corretos: %d/4\n", total_corretos);
    printf("Testes grandes: %s\n", 
           (result5 == AMX_SUCCESS && result6 == AMX_SUCCESS && result7 == AMX_SUCCESS && teste7_ok) 
           ? "✓ TODOS PASSARAM" : "✗ ALGUNS FALHARAM");
    printf("Status geral: %s\n", 
           (total_corretos==4) ? "✓ INT8 FUNCIONANDO COMPLETAMENTE!" : "✗ INT8 AINDA TEM PROBLEMAS");
    printf("\n");
}

void debug_teste5() {
    printf("=== DEBUG TESTE 5 ===\n");
    
    // Teste mais simples primeiro: usar função small para comparar
    printf("Comparação Small vs Large:\n");
    
    // Teste 1: Dimensão que usa função small (M=16, K=64, N=5)
    uint8_t* A_small = malloc(16 * 64 * sizeof(uint8_t));
    int8_t* B_small = malloc(64 * 5 * sizeof(int8_t));
    int32_t* C_small = malloc(16 * 5 * sizeof(int32_t));
    
    for (int i = 0; i < 16 * 64; i++) A_small[i] = 2;
    for (int i = 0; i < 64 * 5; i++) B_small[i] = 1;
    
    amx_multiply_uint8_int8_to_int32(A_small, B_small, C_small, 16, 64, 5);
    printf("Small (16x64x64x5): C[0][0] = %d (esperado: 128)\n", C_small[0]);
    
    // Teste 2: Dimensão que força função large (M=20, K=100, N=5)
    uint8_t* A_large = malloc(20 * 100 * sizeof(uint8_t));
    int8_t* B_large = malloc(100 * 5 * sizeof(int8_t));
    int32_t* C_large = malloc(20 * 5 * sizeof(int32_t));
    
    for (int i = 0; i < 20 * 100; i++) A_large[i] = 2;
    for (int i = 0; i < 100 * 5; i++) B_large[i] = 1;
    
    amx_multiply_uint8_int8_to_int32(A_large, B_large, C_large, 20, 100, 5);
    printf("Large (20x100x100x5): C[0][0] = %d (esperado: 200)\n", C_large[0]);
    printf("Large: C[15][0] = %d, C[19][4] = %d\n", C_large[15*5+0], C_large[19*5+4]);
    
    // Teste 3: CPU reference para conferir
    int32_t cpu_result = 0;
    for (int k = 0; k < 100; k++) {
        cpu_result += A_large[0 * 100 + k] * B_large[k * 5 + 0];
    }
    printf("CPU reference: %d\n", cpu_result);
    
    free(A_small); free(B_small); free(C_small);
    free(A_large); free(B_large); free(C_large);
}

void test_int8_int8_incompleto() {
    printf("=======================================\n");
    printf("==== TESTE INT8 x INT8  INCOMPLETO ====\n");
    printf("=======================================\n");

    // Teste 1: Valores positivos pequenos
    int8_t A1[] = {2, 3};
    int8_t B1[] = {1, 4};
    int32_t C1[1] = {0};
    printf("Teste 1: A=[2,3], B=[1,4] (esperado: 2*1 + 3*4 = 14)\n");
    amx_multiply_small_int8_int8_to_int32(A1, B1, C1, 1, 2, 1);
    printf("Resultado: %d - %s\n\n", C1[0], (C1[0] == 14) ? "✓ CORRETO" : "✗ INCORRETO");
    
    // Teste 2: Com valores negativos
    int8_t A2[] = {-2, 3};
    int8_t B2[] = {1, -4};
    int32_t C2[1] = {0};
    printf("Teste 2: A=[-2,3], B=[1,-4] (esperado: -2*1 + 3*(-4) = -2 - 12 = -14)\n");
    amx_multiply_small_int8_int8_to_int32(A2, B2, C2, 1, 2, 1);
    printf("Resultado: %d - %s\n\n", C2[0], (C2[0] == -14) ? "✓ CORRETO" : "✗ INCORRETO");
    
    // Teste 3: Valores grandes (mas dentro do range int8: -128 a 127)
    int8_t A3[] = {100, -50};
    int8_t B3[] = {3, 2};
    int32_t C3[1] = {0};
    printf("Teste 3: A=[100,-50], B=[3,2] (esperado: 100*3 + (-50)*2 = 300 - 100 = 200)\n");
    amx_multiply_small_int8_int8_to_int32(A3, B3, C3, 1, 2, 1);
    printf("Resultado: %d - %s\n\n", C3[0], (C3[0] == 200) ? "✓ CORRETO" : "✗ INCORRETO");
    
    // Teste 4: Matriz 2x2
    int8_t A4[] = {1, -2, 3, -4};  // [[1,-2], [3,-4]]
    int8_t B4[] = {5, -6, 7, -8};  // [[5,-6], [7,-8]]
    int32_t C4[4] = {0};
    printf("Teste 4: A=[[1,-2],[3,-4]], B=[[5,-6],[7,-8]]\n");
    printf("Esperado: C[0][0]=1*5+(-2)*7=-9, C[0][1]=1*(-6)+(-2)*(-8)=10\n");
    printf("         C[1][0]=3*5+(-4)*7=-13, C[1][1]=3*(-6)+(-4)*(-8)=14\n");
    amx_multiply_small_int8_int8_to_int32(A4, B4, C4, 2, 2, 2);
    printf("Resultado: C[0][0]=%d, C[0][1]=%d, C[1][0]=%d, C[1][1]=%d\n", C4[0], C4[1], C4[2], C4[3]);
    int corretos = (C4[0]==-9) + (C4[1]==10) + (C4[2]==-13) + (C4[3]==14);
    printf("Status: %d/4 corretos - %s\n\n", corretos, (corretos==4) ? "✓ CORRETO" : "✗ INCORRETO");
}

///////////////////////////////////////
////////////  16uint e 16int //////////
///////////////////////////////////////

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

void test_amx_16int() {
    printf("=========================================\n");
    printf("==== TESTE UINT16 x INT16 INCOMPLETO ====\n");
    printf("=========================================\n");
    
    int M = 2, K = 4, N = 2;  // K=4 (múltiplo de 4)
    
    uint16_t A[] = {
        1, 2, 3, 0,  // Padding com 0
        4, 5, 6, 0
    };
    
    int16_t B[] = {
        1, 2,
        3, 4,
        5, 6,
        0, 0   // Padding com 0
    };
    
    int32_t C[4] = {0};
    
    printf("Testando com K=4 (padded)...\n");
    int result = amx_multiply_uint16_int16_to_int32(A, B, C, M, K, N);
    printf("Resultado: C[0][0]=%d, C[0][1]=%d, C[1][0]=%d, C[1][1]=%d\n", C[0], C[1], C[2], C[3]);
    int corretos = (C[0]==22) + (C[1]==28) + (C[2]==49) + (C[3]==64);
    printf("Status: %d/4 corretos - %s\n\n", corretos, (corretos==4) ? "✓ CORRETO" : "✗ INCORRETO");
}

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
