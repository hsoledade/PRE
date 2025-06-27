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

int amx_multiply_small_int8_to_int32(const uint8_t* A, const int8_t* B, int32_t* C, int M, int K, int N);
int amx_multiply_large_int8_to_int32(const uint8_t* A, const int8_t* B, int32_t* C, int M, int K, int N);

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

// Setup AMX tile configuration
static void setup_amx_tiles(__tilecfg *cfg) {
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
int amx_multiply_int8_to_int32(const uint8_t* A, const int8_t* B, int32_t* C, int M, int K, int N) {
    if (!amx_initialized) return AMX_ERROR_NOT_INITIALIZED;
    if (!A || !B || !C || M <= 0 || K <= 0 || N <= 0) return AMX_ERROR_INVALID_PARAMS;
    
    // For small matrices, use optimized single-call version
    if (M <= 16 && K <= 64) {
        return amx_multiply_small_int8_to_int32(A, B, C, M, K, N);
    } else {
        return amx_multiply_large_int8_to_int32(A, B, C, M, K, N);
    }
}

// Optimized multiplication for small matrices (M ≤ 16, K ≤ 64)
int amx_multiply_small_int8_to_int32(const uint8_t* A, const int8_t* B, int32_t* C, int M, int K, int N) {
    if (!amx_initialized) return AMX_ERROR_NOT_INITIALIZED;
    if (M > 16 || K > 64) return AMX_ERROR_INVALID_PARAMS;
    
    //printf("DEBUG: Entering function with M=%d, K=%d, N=%d\n", M, K, N);
    
    // Pad K to multiple of 4 (AMX requirement)
    int K_padded = ((K + 3) / 4) * 4;
    //printf("DEBUG: K_padded=%d\n", K_padded);
    
    uint8_t A_buf[16 * 64] = {0};
    int8_t B_col[16 * 64] = {0};
    int32_t C_col[16 * 16] = {0};
    
    //printf("DEBUG: Buffers allocated\n");
    
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
    
    //printf("DEBUG: Matrix A prepared\n");
    
    // Setup tiles
    __tilecfg cfg = {0};
    setup_amx_tiles(&cfg);
    
    //printf("DEBUG: Tiles configured\n");
    
    // Process each column of C separately
    for (int j = 0; j < N && j < 16; j++) {
        //printf("DEBUG: Processing column %d\n", j);
        
        // Prepare column j of B with padding
        memset(B_col, 0, 16 * 64);
        
        // Each "row k" of B must contain element B[k][j] replicated 4 times
        for (int k = 0; k < K && k < 64; k++) {
            if ((k * 64 + 3) < (16 * 64)) {  // Bounds check
                B_col[k * 64 + 0] = B[k * N + j];
                B_col[k * 64 + 1] = B[k * N + j];
                B_col[k * 64 + 2] = B[k * N + j];
                B_col[k * 64 + 3] = B[k * N + j];
            }
        }
        
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

// Block-based multiplication for large matrices
int amx_multiply_large_int8_to_int32(const uint8_t* A, const int8_t* B, int32_t* C, int M, int K, int N) {
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
                uint8_t* A_block = malloc(mb * kb);
                if (!A_block) {
                    free(C_col);
                    return AMX_ERROR_INVALID_PARAMS;
                }

                for (int i = 0; i < mb; i++) {
                    for (int k = 0; k < kb; k++) {
                        A_block[i * kb + k] = A[(i0 + i) * K + (k0 + k)];
                    }
                }

                // Prepare column B_col (kb elements replicated 4 times)
                int8_t* B_col = calloc(64 * 16, sizeof(int8_t));  // same layout as used by amx_multiply_small
                if (!B_col) {
                    free(A_block);
                    free(C_col);
                    return AMX_ERROR_INVALID_PARAMS;
                }

                for (int k = 0; k < kb; k++) {
                    int8_t val = B[(k0 + k) * N + j0];
                    for (int r = 0; r < 4; r++) {
                        B_col[k * 64 + r] = val;
                    }
                }

                // Prepare temporary buffer C_tile (mb × 1)
                int32_t* C_tile = calloc(16 * 16, sizeof(int32_t));  // must be 16×16 due to _tile_stored requirements
                if (!C_tile) {
                    free(A_block); free(B_col); free(C_col);
                    return AMX_ERROR_INVALID_PARAMS;
                }

                __tilecfg cfg = {0};
                setup_amx_tiles(&cfg);

                _tile_loadd(2, A_block, 64);   // A_tile
                _tile_loadd(3, B_col, 64);     // B_tile
                _tile_loadd(1, C_tile, 64);    // C_tile
                _tile_dpbssd(1, 2, 3);
                _tile_stored(1, C_tile, 64);
                _tile_release();

                // Accumulate partial result into the column accumulator
                for (int i = 0; i < mb; i++) {
                    C_col[i] += C_tile[i * 16];
                }

                free(A_block); free(B_col); free(C_tile);
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

#endif // AMX_MATRIX_H
