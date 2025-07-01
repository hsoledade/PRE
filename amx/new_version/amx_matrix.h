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

/*
Tabela completa das instruções AMX:
Matriz A    Matriz B    Instrução       Significado
uint8_t     uint8_t     _tile_dpbuud    unsigned × unsigned → unsigned
uint8_t     int8_t      _tile_dpbusd    unsigned × signed → signed  
int8_t      uint8_t     _tile_dpbsud    signed × unsigned → signed
int8_t      int8_t      _tile_dpbssd    signed × signed → signed
*/

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
        printf("Small:\n");
        return amx_multiply_small_uint8_int8_to_int32(A, B, C, M, K, N);
    } else {
        printf("Large:\n");
        return amx_multiply_large_uint8_int8_to_int32(A, B, C, M, K, N);
    }
}

// Optimized multiplication for small matrices (M ≤ 16, K ≤ 64)
int amx_multiply_small_uint8_int8_to_int32(const uint8_t* A, const int8_t* B, int32_t* C, int M, int K, int N) {
    if (!amx_initialized) return AMX_ERROR_NOT_INITIALIZED;
    if (M > 16 || K > 64) return AMX_ERROR_INVALID_PARAMS; //"Each tile has a maximum size of 16 rows by 64 bytes"
    
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
        _tile_dpbusd(1, 2, 3);
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
    if (M > 16 || K > 64) return AMX_ERROR_INVALID_PARAMS; // "Each tile has a maximum size of 16 rows by 64 bytes"

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

// Block-based multiplication for large matrices - SIMPLIFICADA
int amx_multiply_large_uint8_int8_to_int32(const uint8_t* A, const int8_t* B, int32_t* C, int M, int K, int N) {
    if (!amx_initialized) return AMX_ERROR_NOT_INITIALIZED;

    memset(C, 0, M * N * sizeof(int32_t));

    // Buffers fixos - sem malloc/free
    uint8_t A_block[16 * 64] = {0};
    int8_t B_col[64] = {0};
    int32_t C_tile[16 * 16] = {0};
    int32_t C_col[16] = {0};

    // Configurar tiles uma vez
    __tilecfg cfg = {0};
    setup_amx_tiles_8int(&cfg);

    // Processar em blocos de 16 linhas
    for (int i0 = 0; i0 < M; i0 += 16) {
        int mb = (M - i0 >= 16) ? 16 : M - i0;

        // Processar cada coluna separadamente
        for (int j0 = 0; j0 < N; j0++) {
            
            // Zerar acumulador da coluna
            memset(C_col, 0, sizeof(C_col));

            // Processar em blocos de K=64
            for (int k0 = 0; k0 < K; k0 += 64) {
                int kb = (K - k0 >= 64) ? 64 : K - k0;

                // Limpar buffers
                memset(A_block, 0, sizeof(A_block));
                memset(B_col, 0, sizeof(B_col));
                memset(C_tile, 0, sizeof(C_tile));

                // Preparar bloco A (mb × kb)
                for (int i = 0; i < mb; i++) {
                    for (int k = 0; k < kb; k++) {
                        A_block[i * 64 + k] = A[(i0 + i) * K + (k0 + k)];
                    }
                }

                // Preparar coluna B (layout contíguo)
                for (int k = 0; k < kb; k++) {
                    B_col[k] = B[(k0 + k) * N + j0];
                }

                // Executar AMX
                _tile_loadd(2, A_block, 64);
                _tile_loadd(3, B_col, 64);
                _tile_loadd(1, C_tile, 64);
                _tile_dpbusd(1, 2, 3);
                _tile_stored(1, C_tile, 64);

                // Acumular resultado parcial
                for (int i = 0; i < mb; i++) {
                    C_col[i] += C_tile[i * 16];
                }
            }

            // Copiar coluna final para matriz C
            for (int i = 0; i < mb; i++) {
                C[(i0 + i) * N + j0] = C_col[i];
            }
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

///////////////////////////////////////
///////////  16uint e 16int ///////////
///////////////////////////////////////

/*
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
*/

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
    // "Each tile has a maximum size of 16 rows by 64 bytes"
    
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
    printf("DEBUG: B_low first elements: %d %d %d %d %d %d\n", B_low[0], B_low[1], B_low[2], B_low[3], B_low[4], B_low[5]);
    
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
