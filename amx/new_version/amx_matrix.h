#ifndef AMX_MATRIX_H
#define AMX_MATRIX_H

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>                
#include <sys/syscall.h>           
#include <immintrin.h>

#ifndef ARCH_REQ_XCOMP_PERM
#define ARCH_REQ_XCOMP_PERM 0x1023
#endif

#define AMX_SUCCESS 0
#define AMX_ERROR_NOT_SUPPORTED -1
#define AMX_ERROR_NOT_INITIALIZED -2
#define AMX_ERROR_INVALID_PARAMS -3

typedef struct __tile_config {
    uint8_t palette_id;
    uint8_t start_row;
    uint8_t reserved_0[14];
    uint16_t colsb[16];
    uint8_t rows[16];
} __tilecfg;

static int amx_initialized = 0;

int amx_multiply_small_uint8_int8_to_int32(const uint8_t* A, const int8_t* B, int32_t* C, int M, int K, int N);
int amx_multiply_large_uint8_int8_to_int32(const uint8_t* A, const int8_t* B, int32_t* C, int M, int K, int N);
int amx_multiply_small_uint16_int16_to_int32(const uint16_t* A, const int16_t* B, int32_t* C, int M, int K, int N);
//int amx_multiply_large_uint16_int16_to_int32(const uint16_t* A, const int16_t* B, int32_t* C, int M, int K, int N);

// Initialize Intel AMX
int amx_init(void) {
    if (amx_initialized) return AMX_SUCCESS;
    
    if (syscall(SYS_arch_prctl, ARCH_REQ_XCOMP_PERM, 18)) {
        return AMX_ERROR_NOT_SUPPORTED;
    }
    
    amx_initialized = 1;
    return AMX_SUCCESS;
}

int amx_is_initialized(void) {
    return amx_initialized;
}

///////////////////////////////////////
////////////  8uint e 8int ////////////
///////////////////////////////////////

// Setup AMX tile configuration
static void setup_amx_tiles_8int(__tilecfg *cfg) {
    cfg->palette_id = 1;
    cfg->start_row = 0;
    cfg->colsb[0] = 16;    cfg->rows[0] = 16;
    cfg->colsb[1] = 64;    cfg->rows[1] = 16;
    cfg->colsb[2] = 64;    cfg->rows[2] = 16;
    cfg->colsb[3] = 64;    cfg->rows[3] = 16;
    
    _tile_loadconfig(cfg);
}

/**
 * Computes C = A × B where:
 * - A is M×K matrix of unsigned 8-bit integers
 * - B is K×N matrix of signed 8-bit integers  
 * - C is M×N matrix of 32-bit integers (output)
 */
int amx_multiply_uint8_int8_to_int32(const uint8_t* A, const int8_t* B, int32_t* C, int M, int K, int N) {
    if (!amx_initialized) return AMX_ERROR_NOT_INITIALIZED;
    if (!A || !B || !C || M <= 0 || K <= 0 || N <= 0) return AMX_ERROR_INVALID_PARAMS;
    
    // For small matrices, use optimized single-call version
    if (M <= 16 && K <= 64) {
        return amx_multiply_small_uint8_int8_to_int32(A, B, C, M, K, N);
    } else {
        return amx_multiply_large_uint8_int8_to_int32(A, B, C, M, K, N);
    }
}

// Optimized multiplication for small matrices (M ≤ 16, K ≤ 64)
int amx_multiply_small_uint8_int8_to_int32(const uint8_t* A, const int8_t* B, int32_t* C, int M, int K, int N) {
    if (!amx_initialized) return AMX_ERROR_NOT_INITIALIZED;
    if (M > 16 || K > 64) return AMX_ERROR_INVALID_PARAMS;
    
    //printf("DEBUG: Entering function with M=%d, K=%d, N=%d\n", M, K, N);
    
    // Pad K to multiple of 4 (AMX requirement)
    //int K_padded = ((K + 3) / 4) * 4;
    //printf("DEBUG: K_padded=%d\n", K_padded);
    
    uint8_t A_buf[16 * 64] = {0};
    int8_t B_col[16 * 64] = {0};
    int32_t C_col[16 * 16] = {0};
    
    //printf("DEBUG: Buffers allocated\n");
    
    /*
    // Prepare matrix A with padding
    for (int i = 0; i < M && i < 16; i++) {
        for (int k = 0; k < K && k < 64; k++) {
            A_buf[i * 64 + k] = A[i * K + k];
        }
        // Zero padding for remaining elements
        for (int k = K; k < K_padded && k < 64; k++) {
            A_buf[i * 64 + k] = 0;
        }
    }
    */
    for (int i = 0; i < M && i < 16; i++) {
        for (int k = 0; k < K && k < 64; k++) {
            A_buf[i * 64 + k] = A[i * K + k];
        }
    }
    
    //printf("DEBUG: Matrix A prepared\n");
    
    // Setup tiles
    __tilecfg cfg = {0};
    setup_amx_tiles_8int(&cfg);
    
    //printf("DEBUG: Tiles configured\n");
    
    // Process each column of C separately
    for (int j = 0; j < N && j < 16; j++) {
        //printf("DEBUG: Processing column %d\n", j);
        
        // Prepare column j of B with padding
        memset(B_col, 0, 16 * 64);
        
        /*
        // Each "row k" of B must contain element B[k][j] replicated 4 times
        for (int k = 0; k < K && k < 64; k++) {
            if ((k * 64 + 3) < (16 * 64)) {  // Bounds check
                B_col[k * 64 + 0] = B[k * N + j];
                B_col[k * 64 + 1] = B[k * N + j];
                B_col[k * 64 + 2] = B[k * N + j];
                B_col[k * 64 + 3] = B[k * N + j];
            }
        }
        */
        
        // Teste: layout contíguo simples
        for (int k = 0; k < K && k < 64; k++) {
            B_col[k] = B[k * N + j];          }
        
        //printf("DEBUG: Column B prepared\n");
        
        // Clear column result
        memset(C_col, 0, 16 * 16 * sizeof(int32_t));
        
        //printf("DEBUG: Executing AMX...\n");
        
        _tile_loadd(2, A_buf, 64);
        _tile_loadd(3, B_col, 64);
        _tile_loadd(1, C_col, 64);
        _tile_dpbssd(1, 2, 3);
        _tile_stored(1, C_col, 64);
        
        //printf("DEBUG: AMX executed, result=%d\n", C_col[0]);
        
        for (int i = 0; i < M && i < 16; i++) {
            if ((i * N + j) < (M * N)) {  // Bounds check
                C[i * N + j] = C_col[i * 16];
            }
        }
        
        //printf("DEBUG: Column %d copied\n", j);
    }
    
    _tile_release();
    //printf("DEBUG: Function completed successfully\n");
    return AMX_SUCCESS;
}

int amx_multiply_small_int8_int8_to_int32(const int8_t* A, const int8_t* B, int32_t* C, int M, int K, int N) {
    if (!amx_initialized) return AMX_ERROR_NOT_INITIALIZED;
    if (M > 16 || K > 64) return AMX_ERROR_INVALID_PARAMS;
    
    // Mesma lógica, mas A também é int8_t
    int8_t A_buf[16 * 64] = {0};
    int8_t B_col[16 * 64] = {0};
    int32_t C_col[16 * 16] = {0};
    
    // Prepare matrix A - agora é int8_t
    for (int i = 0; i < M && i < 16; i++) {
        for (int k = 0; k < K && k < 64; k++) {
            A_buf[i * 64 + k] = A[i * K + k];
        }
    }
    
    __tilecfg cfg = {0};
    setup_amx_tiles_8int(&cfg);
    
    for (int j = 0; j < N && j < 16; j++) {
        memset(B_col, 0, 16 * 64);
        
        // Layout contíguo (como corrigimos)
        for (int k = 0; k < K && k < 64; k++) {
            B_col[k] = B[k * N + j];
        }
        
        memset(C_col, 0, 16 * 16 * sizeof(int32_t));
        
        _tile_loadd(2, A_buf, 64); 
        _tile_loadd(3, B_col, 64);
        _tile_loadd(1, C_col, 64);
        _tile_dpbssd(1, 2, 3);  // signed × signed → signed
        _tile_stored(1, C_col, 64);
        
        for (int i = 0; i < M && i < 16; i++) {
            C[i * N + j] = C_col[i * 16];
        }
    }
    
    _tile_release();
    return AMX_SUCCESS;
}

 /*
    Para matrizes pequenas (como seus testes M=2, K=3, N=2):

    AMX é MAIS LENTO - overhead de configuração é maior que o ganho
    CPU simples é mais rápido - 6 multiplicações simples são instantâneas

    Para matrizes grandes (M≥16, K≥64):

    AMX é MUITO mais rápido - paralelização massiva compensa o overhead
    CPU simples fica lento - O(M×K×N) operações sequenciais

    // O que AMX faz por operação:
    _tile_loadconfig()     // ~100 ciclos
    _tile_loadd() × 3      // ~30 ciclos cada
    _tile_dpbssd()         // ~1 ciclo (mas faz centenas de ops)
    _tile_stored()         // ~30 ciclos  
    _tile_release()        // ~20 ciclos
    // Total: ~200+ ciclos de overhead
*/

// Block-based multiplication for large matrices - CORRIGIDA
int amx_multiply_large_uint8_int8_to_int32(const uint8_t* A, const int8_t* B, int32_t* C, int M, int K, int N) {
    if (!amx_initialized) return AMX_ERROR_NOT_INITIALIZED;

    //printf("DEBUG LARGE: M=%d, K=%d, N=%d\n", M, K, N);

    memset(C, 0, M * N * sizeof(int32_t));

    for (int i0 = 0; i0 < M; i0 += 16) {
        int mb = (M - i0 >= 16) ? 16 : M - i0;

        for (int j0 = 0; j0 < N; j0++) {
            // Allocate and initialize the column accumulator
            int32_t* C_col = calloc(mb, sizeof(int32_t));
            if (!C_col) return AMX_ERROR_INVALID_PARAMS;

            for (int k0 = 0; k0 < K; k0 += 64) {
                int kb = (K - k0 >= 64) ? 64 : K - k0;

                // Prepare submatrix A_block (mb × kb)
                uint8_t* A_block = malloc(mb * 64 * sizeof(uint8_t));  // Sempre aloca com stride 64
                if (!A_block) {
                    free(C_col);
                    return AMX_ERROR_INVALID_PARAMS;
                }
                
                // Zero padding do A_block
                memset(A_block, 0, mb * 64 * sizeof(uint8_t));
                
                for (int i = 0; i < mb; i++) {
                    for (int k = 0; k < kb; k++) {
                        A_block[i * 64 + k] = A[(i0 + i) * K + (k0 + k)];  // Stride 64 para AMX
                    }
                    // Padding automático com memset acima
                }

                // Prepare column B_col - LAYOUT CONTÍGUO CORRETO
                int8_t* B_col = calloc(64, sizeof(int8_t));  // Só precisa de kb elementos
                if (!B_col) {
                    free(A_block);
                    free(C_col);
                    return AMX_ERROR_INVALID_PARAMS;
                }

                // CORREÇÃO PRINCIPAL: layout contíguo em vez de replicação
                for (int k = 0; k < kb; k++) {
                    int8_t val = B[(k0 + k) * N + j0];
                    B_col[k] = val;  // LAYOUT CONTÍGUO SIMPLES
                }
                // Elementos kb até 63 já são zero devido ao calloc

                // Prepare temporary buffer C_tile (mb × 1)
                int32_t* C_tile = calloc(16 * 16, sizeof(int32_t));  // Must be 16×16 due to _tile_stored requirements
                if (!C_tile) {
                    free(A_block); 
                    free(B_col); 
                    free(C_col);
                    return AMX_ERROR_INVALID_PARAMS;
                }

                __tilecfg cfg = {0};
                setup_amx_tiles_8int(&cfg);

                _tile_loadd(2, A_block, 64);    // A_tile - sempre stride 64
                _tile_loadd(3, B_col, 64);      // B_tile - stride 64 
                _tile_loadd(1, C_tile, 64);     // C_tile - stride 64
                _tile_dpbssd(1, 2, 3);
                _tile_stored(1, C_tile, 64);
                _tile_release();

                // Accumulate partial result into the column accumulator
                for (int i = 0; i < mb; i++) {
                    C_col[i] += C_tile[i * 16];
                }

                free(A_block); 
                free(B_col); 
                free(C_tile);
            }

            // Copy the final accumulated column to matrix C
            for (int i = 0; i < mb; i++) {
                C[(i0 + i) * N + j0] = C_col[i];
            }

            free(C_col);
        }
    }

    return AMX_SUCCESS;
}

void test_int8_completo() {
    printf("=== TESTE INT8 COMPLETO ===\n");
    
    // Teste 1: Caso que estava falhando
    uint8_t A1[] = {2, 3};
    int8_t B1[] = {1, 4};
    int32_t C1[1] = {0};
    printf("Teste 1: A=[2,3], B=[1,4] (esperado: 2*1 + 3*4 = 14)\n");
    amx_multiply_small_uint8_int8_to_int32(A1, B1, C1, 1, 2, 1);
    printf("Resultado: %d - %s\n\n", C1[0], (C1[0] == 14) ? "✓ CORRETO" : "✗ INCORRETO");
    
    // Teste 2: Matriz 2x2 × vetor 2x1 
    uint8_t A2[] = {1, 2, 3, 4};  // [[1,2], [3,4]]
    int8_t B2[] = {5, 6};         // [[5], [6]]
    int32_t C2[2] = {0};
    printf("Teste 2: A=[[1,2],[3,4]], B=[[5],[6]] (esperado: [17, 39])\n");
    printf("Cálculo: [1*5+2*6, 3*5+4*6] = [5+12, 15+24] = [17, 39]\n");
    amx_multiply_small_uint8_int8_to_int32(A2, B2, C2, 2, 2, 1);
    printf("Resultado: [%d, %d] - %s\n\n", C2[0], C2[1], 
           (C2[0]==17 && C2[1]==39) ? "✓ CORRETO" : "✗ INCORRETO");
    
    // Teste 3: Matriz 2x3 × matriz 3x2 (como no teste 16-bit)
    uint8_t A3[] = {1, 2, 3, 4, 5, 6};  // [[1,2,3], [4,5,6]]
    int8_t B3[] = {1, 2, 3, 4, 5, 6};   // [[1,2], [3,4], [5,6]]
    int32_t C3[4] = {0};
    printf("Teste 3: A=[[1,2,3],[4,5,6]], B=[[1,2],[3,4],[5,6]]\n");
    printf("Esperado: C[0][0]=22, C[0][1]=28, C[1][0]=49, C[1][1]=64\n");
    printf("Cálculo C[0][0]: 1*1 + 2*3 + 3*5 = 1 + 6 + 15 = 22\n");
    printf("Cálculo C[0][1]: 1*2 + 2*4 + 3*6 = 2 + 8 + 18 = 28\n");
    amx_multiply_small_uint8_int8_to_int32(A3, B3, C3, 2, 3, 2);
    printf("Resultado: C[0][0]=%d, C[0][1]=%d, C[1][0]=%d, C[1][1]=%d\n", 
           C3[0], C3[1], C3[2], C3[3]);
    int corretos = (C3[0]==22) + (C3[1]==28) + (C3[2]==49) + (C3[3]==64);
    printf("Status: %d/4 corretos - %s\n\n", corretos, (corretos==4) ? "✓ CORRETO" : "✗ INCORRETO");
    
    // Teste 4: Valores que usam byte alto (>255 quando combinados)
    uint8_t A4[] = {100, 200};
    int8_t B4[] = {3, 2};
    int32_t C4[1] = {0};
    printf("Teste 4: A=[100,200], B=[3,2] (esperado: 100*3 + 200*2 = 700)\n");
    amx_multiply_small_uint8_int8_to_int32(A4, B4, C4, 1, 2, 1);
    printf("Resultado: %d - %s\n\n", C4[0], (C4[0] == 700) ? "✓ CORRETO" : "✗ INCORRETO");
    
    printf("=== RESUMO TESTE INT8 ===\n");
    int total_corretos = (C1[0]==14) + (C2[0]==17 && C2[1]==39) + (corretos==4) + (C4[0]==700);
    printf("Testes corretos: %d/4\n", total_corretos);
    printf("Status geral: %s\n", (total_corretos==4) ? "✓ INT8 FUNCIONANDO!" : "✗ INT8 AINDA TEM PROBLEMAS");
}

void test_int8_int8() {
    printf("=== TESTE INT8 × INT8 ===\n");
    
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
    printf("Resultado: C[0][0]=%d, C[0][1]=%d, C[1][0]=%d, C[1][1]=%d\n", 
           C4[0], C4[1], C4[2], C4[3]);
    int corretos = (C4[0]==-9) + (C4[1]==10) + (C4[2]==-13) + (C4[3]==14);
    printf("Status: %d/4 corretos - %s\n\n", corretos, (corretos==4) ? "✓ CORRETO" : "✗ INCORRETO");
}

///////////////////////////////////////
///////////  16uint e 16int ///////////
///////////////////////////////////////

// TO REPAIR :
// TO REPAIR :
// TO REPAIR :

// Setup AMX tiles for 16-bit operations
static void setup_amx_tiles_16int(__tilecfg *cfg) {
    cfg->palette_id = 1;
    cfg->start_row = 0;
    cfg->colsb[0] = 64; cfg->rows[0] = 16;  // Tile 0: acumulador (int32)
    cfg->colsb[1] = 64; cfg->rows[1] = 16;  // Tile 1: result
    cfg->colsb[2] = 32; cfg->rows[2] = 16;  // Tile 2: matriz A (int16)
    cfg->colsb[3] = 32; cfg->rows[3] = 16;  // Tile 3: matriz B (int16)
    _tile_loadconfig(cfg);
}


int amx_multiply_uint16_int16_to_int32(const uint16_t* A, const int16_t* B, int32_t* C, int M, int K, int N) {
    if (!amx_initialized) return AMX_ERROR_NOT_INITIALIZED;
    if (!A || !B || !C || M <= 0 || K <= 0 || N <= 0) return AMX_ERROR_INVALID_PARAMS;
    
    // For small matrices, use optimized single-call version
    if (M <= 16 && K <= 32) {  // Note: K limit is 32 for 16-bit (vs 64 for 8-bit)
        return amx_multiply_small_uint16_int16_to_int32(A, B, C, M, K, N);
    } else {
        return AMX_ERROR_INVALID_PARAMS; 
        //return amx_multiply_large_uint16_int16_to_int32(A, B, C, M, K, N);
    }
}

// Optimized multiplication for small matrices (M ≤ 16, K ≤ 32) using 4x int8 operations
int amx_multiply_small_uint16_int16_to_int32(const uint16_t* A, const int16_t* B, int32_t* C, int M, int K, int N) {
    if (!amx_initialized) return AMX_ERROR_NOT_INITIALIZED;
    if (M > 16 || K > 32) return AMX_ERROR_INVALID_PARAMS;  // K limit is 32 for 16-bit
    
    printf("DEBUG: 16-bit function using 4x 8-bit M=%d, K=%d, N=%d\n", M, K, N);
    
    // Allocate buffers for 8-bit decomposition
    uint8_t A_low[16 * 32] = {0};   // Low bytes of A (same dimensions as original)
    uint8_t A_high[16 * 32] = {0};  // High bytes of A
    int8_t B_low[32 * 16] = {0};    // Low bytes of B (same dimensions as original)
    int8_t B_high[32 * 16] = {0};   // High bytes of B
    
    int32_t C_temp[16 * 16] = {0};  // Temporary result for each operation
    
    // Clear output matrix
    memset(C, 0, M * N * sizeof(int32_t));
    
    printf("DEBUG: Decomposing A (uint16 -> uint8 low/high)\n");
    // Decompose A: each uint16 element becomes separate uint8 elements
    for (int i = 0; i < M; i++) {
        for (int k = 0; k < K; k++) {
            uint16_t val = A[i * K + k];
            A_low[i * K + k] = val & 0xFF;        // Low byte
            A_high[i * K + k] = (val >> 8) & 0xFF; // High byte
        }
    }
    
    printf("DEBUG: Decomposing B (int16 -> int8 low/high)\n");
    // Decompose B: each int16 element becomes separate int8 elements
    for (int k = 0; k < K; k++) {
        for (int j = 0; j < N; j++) {
            int16_t val = B[k * N + j];
            B_low[k * N + j] = (int8_t)(val & 0xFF);        // Low byte
            B_high[k * N + j] = (int8_t)((val >> 8) & 0xFF); // High byte
        }
    }
    
    printf("DEBUG: A_low first elements: %d %d %d\n", A_low[0], A_low[1], A_low[2]);
    printf("DEBUG: B_low first elements: %d %d %d %d %d %d\n", 
           B_low[0], B_low[1], B_low[2], B_low[3], B_low[4], B_low[5]);
    
    printf("DEBUG: Starting 4 AMX operations (each using normal K=%d)\n", K);
    
    // Operation 1: A_low × B_low
    printf("DEBUG: Op 1 - A_low × B_low\n");
    memset(C_temp, 0, sizeof(C_temp));
    if (amx_multiply_small_uint8_int8_to_int32(A_low, B_low, C_temp, M, K, N) != AMX_SUCCESS) {
        printf("ERROR: Operation 1 failed\n");
        return AMX_ERROR_INVALID_PARAMS;
    }
    // Add to result (no shift needed)
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            C[i * N + j] += C_temp[i * N + j];
        }
    }
    printf("DEBUG: Op 1 completed, C_temp[0] = %d, C[0][0] = %d\n", C_temp[0], C[0]);
    
    // Operation 2: A_low × B_high (shift left 8)
    printf("DEBUG: Op 2 - A_low × B_high << 8\n");
    memset(C_temp, 0, sizeof(C_temp));
    if (amx_multiply_small_uint8_int8_to_int32(A_low, B_high, C_temp, M, K, N) != AMX_SUCCESS) {
        printf("ERROR: Operation 2 failed\n");
        return AMX_ERROR_INVALID_PARAMS;
    }
    // Add to result with shift left 8
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            C[i * N + j] += C_temp[i * N + j] << 8;
        }
    }
    printf("DEBUG: Op 2 completed, C_temp[0] = %d, C[0][0] = %d\n", C_temp[0], C[0]);
    
    // Operation 3: A_high × B_low (shift left 8)
    printf("DEBUG: Op 3 - A_high × B_low << 8\n");
    memset(C_temp, 0, sizeof(C_temp));
    if (amx_multiply_small_uint8_int8_to_int32(A_high, B_low, C_temp, M, K, N) != AMX_SUCCESS) {
        printf("ERROR: Operation 3 failed\n");
        return AMX_ERROR_INVALID_PARAMS;
    }
    // Add to result with shift left 8
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            C[i * N + j] += C_temp[i * N + j] << 8;
        }
    }
    printf("DEBUG: Op 3 completed, C_temp[0] = %d, C[0][0] = %d\n", C_temp[0], C[0]);
    
    // Operation 4: A_high × B_high (shift left 16)
    printf("DEBUG: Op 4 - A_high × B_high << 16\n");
    memset(C_temp, 0, sizeof(C_temp));
    if (amx_multiply_small_uint8_int8_to_int32(A_high, B_high, C_temp, M, K, N) != AMX_SUCCESS) {
        printf("ERROR: Operation 4 failed\n");
        return AMX_ERROR_INVALID_PARAMS;
    }
    // Add to result with shift left 16
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            C[i * N + j] += C_temp[i * N + j] << 16;
        }
    }
    printf("DEBUG: Op 4 completed, C_temp[0] = %d, C[0][0] = %d\n", C_temp[0], C[0]);
    
    printf("DEBUG: 16-bit function completed successfully using 4x 8-bit ops\n");
    return AMX_SUCCESS;
}

/*
// Optimized multiplication for small matrices (M ≤ 16, K ≤ 32)
int amx_multiply_small_uint16_int16_to_int32(const uint16_t* A, const int16_t* B, int32_t* C, int M, int K, int N) {
    if (!amx_initialized) return AMX_ERROR_NOT_INITIALIZED;
    if (M > 16 || K > 32) return AMX_ERROR_INVALID_PARAMS;  // K limit is 32 for 16-bit
    
    printf("DEBUG: 16-bit function M=%d, K=%d, N=%d\n", M, K, N);
    
    // Pad K to multiple of 2 (AMX requirement for 16-bit)
    int K_padded = ((K + 1) / 2) * 2;
    printf("DEBUG: K_padded=%d\n", K_padded);
    
    // AMX buffers - Note: Different sizes for 16-bit
    uint16_t A_buf[16 * 32] = {0};   // 16 rows × 32 cols (max for 16-bit)
    int16_t B_col[16 * 32] = {0};    // 16 rows × 32 cols 
    int32_t C_col[16 * 16] = {0};    // Output is still 16×16
    
    printf("DEBUG: Buffers allocated\n");
    
    // Prepare matrix A with padding
    for (int i = 0; i < M && i < 16; i++) {
        for (int k = 0; k < K && k < 32; k++) {
            A_buf[i * 32 + k] = A[i * K + k];  // Note: stride is 32 for 16-bit
        }
        // Zero padding for remaining elements
        for (int k = K; k < K_padded && k < 32; k++) {
            A_buf[i * 32 + k] = 0;
        }
    }
    
    printf("DEBUG: Matrix A prepared\n");
    
    // Setup tiles for 16-bit operations
    __tilecfg cfg = {0};
    setup_amx_tiles_16int(&cfg);  // Different setup for 16-bit
    
    printf("DEBUG: Tiles configured for 16-bit\n");
    
    // Process each column of C separately
    for (int j = 0; j < N && j < 16; j++) {
        printf("DEBUG: Processing column %d\n", j);
        
        // Prepare column j of B with padding
        memset(B_col, 0, 16 * 32 * sizeof(int16_t));
        
        // Each "row k" of B must contain element B[k][j] replicated 2 times (vs 4 for 8-bit)
        for (int k = 0; k < K && k < 32; k++) {
            if ((k * 32 + 1) < (16 * 32)) {  // Bounds check
                B_col[k * 32 + 0] = B[k * N + j];
                B_col[k * 32 + 1] = B[k * N + j];
                // Note: Only 2 replications for 16-bit (vs 4 for 8-bit)
            }
        }
        
        printf("DEBUG: Column B prepared\n");
        
        // Clear column result
        memset(C_col, 0, 16 * 16 * sizeof(int32_t));
        
        printf("DEBUG: Executing AMX 16-bit...\n");
        
        // Load tiles - stride is 64 bytes (32 × 2 bytes)
        _tile_loadd(2, A_buf, 64);  // Load A (uint16)
        _tile_loadd(3, B_col, 64);  // Load B (int16)
        _tile_loadd(1, C_col, 64);  // Load C (int32)
        
        // Use 16-bit instruction: _tile_dpbuud (unsigned 16 × signed 16 → signed 32)
        _tile_dpbuud(1, 2, 3);      // C += A × B (16-bit operation)
        
        _tile_stored(1, C_col, 64); // Store result
        
        printf("DEBUG: AMX executed, result=%d\n", C_col[0]);
        
        for (int i = 0; i < M && i < 16; i++) {
            if ((i * N + j) < (M * N)) {  // Bounds check
                C[i * N + j] = C_col[i * 16];
            }
        }
        
        printf("DEBUG: Column %d copied\n", j);
    }
    
    _tile_release();
    printf("DEBUG: 16-bit function completed successfully\n");
    return AMX_SUCCESS;
}
*/

void test() {
    printf("=== TESTE DIRETO DA FUNÇÃO INT8 ===\n");
    
    // Teste: A=[2,3] × B=[[1],[4]] = 2×1 + 3×4 = 14
    uint8_t A_test[] = {2, 3};
    int8_t B_test[] = {1, 4};  // B como vetor: [B[0][0], B[1][0]]
    int32_t C_test[1] = {0};
    
    printf("Teste int8 direto: A=[2,3], B=[1,4], M=1, K=2, N=1\n");
    int result = amx_multiply_small_uint8_int8_to_int32(A_test, B_test, C_test, 1, 2, 1);
    printf("Resultado int8: %d (esperado: 14), status: %d\n", C_test[0], result);
    
    // Teste ainda mais simples: A=[1] × B=[2] = 2
    uint8_t A_simple[] = {1};
    int8_t B_simple[] = {2};
    int32_t C_simple[1] = {0};
    
    printf("Teste ultra-simples: A=[1], B=[2], M=1, K=1, N=1\n");
    result = amx_multiply_small_uint8_int8_to_int32(A_simple, B_simple, C_simple, 1, 1, 1);
    printf("Resultado ultra-simples: %d (esperado: 2), status: %d\n", C_simple[0], result);
    
    printf("=== FIM TESTE INT8 DIRETO ===\n\n");

    // Teste: A=[2,3,0,0] × B=[[1],[4],[0],[0]] = 2×1 + 3×4 + 0×0 + 0×0 = 14
    uint8_t A_test4[] = {2, 3, 0, 0};
    int8_t B_test4[] = {1, 4, 0, 0};
    int32_t C_test4[1] = {0};

    printf("Teste int8 K=4: A=[2,3,0,0], B=[1,4,0,0]\n");
    result = amx_multiply_small_uint8_int8_to_int32(A_test4, B_test4, C_test4, 1, 4, 1);
    printf("Resultado K=4: %d (esperado: 14)\n", C_test4[0]);

    uint8_t A_debug[] = {1, 0};  // Teste: [1,0] × [2,3] = 1*2 + 0*3 = 2
    int8_t B_debug[] = {2, 3};
    int32_t C_debug[1] = {0};

    printf("Teste debug: A=[1,0], B=[2,3]\n");
    result = amx_multiply_small_uint8_int8_to_int32(A_debug, B_debug, C_debug, 1, 2, 1);
    printf("Resultado debug: %d (esperado: 2)\n", C_debug[0]);

    uint8_t A_debug2[] = {0, 1};  // Teste: [0,1] × [2,3] = 0*2 + 1*3 = 3
    int8_t B_debug2[] = {2, 3};
    int32_t C_debug2[1] = {0};

    printf("Teste debug2: A=[0,1], B=[2,3]\n");
    result = amx_multiply_small_uint8_int8_to_int32(A_debug2, B_debug2, C_debug2, 1, 2, 1);
    printf("Resultado debug2: %d (esperado: 3)\n", C_debug2[0]);
}

void test_k3() {
    printf("=== TESTE K=3 ===\n");
    
    // Teste A=[1,2,3] × B=[[4],[5],[6]] = 1*4 + 2*5 + 3*6 = 4 + 10 + 18 = 32
    uint8_t A[] = {1, 2, 3};
    int8_t B[] = {4, 5, 6};
    int32_t C[1] = {0};
    
    printf("Teste K=3: A=[1,2,3], B=[4,5,6] (esperado: 1*4 + 2*5 + 3*6 = 32)\n");
    int result = amx_multiply_small_uint8_int8_to_int32(A, B, C, 1, 3, 1);
    printf("Resultado: %d - %s\n", C[0], (C[0] == 32) ? "✓ CORRETO" : "✗ INCORRETO");
    
    // Teste K=4 (múltiplo de 4)
    uint8_t A4[] = {1, 2, 3, 4};
    int8_t B4[] = {2, 3, 4, 5};
    int32_t C4[1] = {0};
    
    printf("Teste K=4: A=[1,2,3,4], B=[2,3,4,5] (esperado: 1*2 + 2*3 + 3*4 + 4*5 = 40)\n");
    result = amx_multiply_small_uint8_int8_to_int32(A4, B4, C4, 1, 4, 1);
    printf("Resultado: %d - %s\n", C4[0], (C4[0] == 40) ? "✓ CORRETO" : "✗ INCORRETO");
    
    // Relembre K=1 e K=2 para comparar
    uint8_t A1[] = {1};
    int8_t B1[] = {2};
    int32_t C1[1] = {0};
    amx_multiply_small_uint8_int8_to_int32(A1, B1, C1, 1, 1, 1);
    printf("Relembrar K=1: A=[1], B=[2] → %d (esperado: 2) %s\n", 
           C1[0], (C1[0] == 2) ? "✓" : "✗");
    
    uint8_t A2[] = {2, 3};
    int8_t B2[] = {1, 4};
    int32_t C2[1] = {0};
    amx_multiply_small_uint8_int8_to_int32(A2, B2, C2, 1, 2, 1);
    printf("Relembrar K=2: A=[2,3], B=[1,4] → %d (esperado: 14) %s\n", 
           C2[0], (C2[0] == 14) ? "✓" : "✗");
}

void test_amx_16int() {
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
    
    if (result == AMX_SUCCESS) {
        printf("Resultado K=4:\n");
        printf("C[0][0] = %d (esperado: 22)\n", C[0]);
        printf("C[0][1] = %d (esperado: 28)\n", C[1]); 
        printf("C[1][0] = %d (esperado: 49)\n", C[2]);
        printf("C[1][1] = %d (esperado: 64)\n", C[3]);
    }
}

/*
// Block-based multiplication for large matrices (16-bit version)
int amx_multiply_large_uint16_int16_to_int32(const uint16_t* A, const int16_t* B, int32_t* C, int M, int K, int N) {
    if (!amx_initialized) return AMX_ERROR_NOT_INITIALIZED;

    printf("DEBUG LARGE 16-bit: M=%d, K=%d, N=%d\n", M, K, N);

    memset(C, 0, M * N * sizeof(int32_t));

    for (int i0 = 0; i0 < M; i0 += 16) {
        for (int j0 = 0; j0 < N; j0++) {
            printf("DEBUG LARGE: Bloco i0=%d, j0=%d\n", i0, j0);
            
            // Allocate column accumulator
            int32_t* C_col = calloc(16, sizeof(int32_t));
            if (!C_col) {
                printf("ERRO: calloc C_col falhou!\n");
                return AMX_ERROR_INVALID_PARAMS;
            }

            int mb = (M - i0 >= 16) ? 16 : M - i0;

            for (int k0 = 0; k0 < K; k0 += 32) {  // Block size 32 for 16-bit
                int kb = (K - k0 >= 32) ? 32 : K - k0;
                
                printf("DEBUG LARGE: Sub-bloco k0=%d, mb=%d, kb=%d\n", k0, mb, kb);

                // Use fixed buffers with proper stride for 16-bit
                uint16_t A_block[16 * 32] = {0};
                int16_t B_col[32 * 16] = {0};     
                int32_t C_tile[16 * 16] = {0};

                // Prepare A_block with padding
                for (int i = 0; i < mb; i++) {
                    for (int k = 0; k < kb; k++) {
                        A_block[i * 32 + k] = A[(i0 + i) * K + (k0 + k)];
                    }
                    // Zero padding
                    for (int k = kb; k < 32; k++) {
                        A_block[i * 32 + k] = 0;
                    }
                }

                // Prepare B_col with padding
                for (int k = 0; k < kb; k++) {
                    for (int r = 0; r < 2; r++) {  // Replicate 2 times for 16-bit
                        B_col[k * 32 + r] = B[(k0 + k) * N + j0];
                    }
                }

                printf("DEBUG LARGE: Executando AMX 16-bit...\n");

                __tilecfg cfg = {0};
                setup_amx_tiles_16int(&cfg);

                _tile_loadd(2, A_block, 64);  // Stride = 64 bytes
                _tile_loadd(3, B_col, 64);
                _tile_loadd(1, C_tile, 64);
                _tile_dpbuud(1, 2, 3);        // 16-bit operation
                _tile_stored(1, C_tile, 64);
                _tile_release();

                printf("DEBUG LARGE: AMX OK, C_tile[0]=%d\n", C_tile[0]);

                // Accumulate results
                for (int i = 0; i < mb; i++) {
                    C_col[i] += C_tile[i * 16];
                }
            }

            // Copy results to final matrix
            for (int i = 0; i < mb; i++) {
                C[(i0 + i) * N + j0] = C_col[i];
            }

            free(C_col);
        }
    }

    return AMX_SUCCESS;
}
*/

#endif // AMX_MATRIX_H
