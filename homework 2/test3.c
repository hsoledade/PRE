#define __AMX_TILE__
#include <immintrin.h>
#include <sys/prctl.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>


#define M 16
#define K 64
#define N 16

int main() {
    // Habilita AMX
    if (prctl(0x53, 0x1) != 0) {
        perror("prctl AMX failed");
        return 1;
    }

    uint8_t  A[M * K];
    int8_t   B[K * N];
    int32_t  C[M * N];

    // Preenche A e B com valores simples
    for (int i = 0; i < M; i++)
        for (int k = 0; k < K; k++)
            A[i * K + k] = (uint8_t)(i + k);

    for (int k = 0; k < K; k++)
        for (int j = 0; j < N; j++)
            B[k * N + j] = (int8_t)(k - j);

    memset(C, 0, sizeof(C));

    // Configuracao minima de tile
    struct {
        uint8_t palette_id;
        uint8_t reserved[15];
        struct { uint16_t rows; uint16_t colsb; } t[8];
    } __attribute__((packed, aligned(64))) cfg = {
        .palette_id = 1,
        .t = {
            {M, K},   // t0 = A (16x64)
            {K, N},   // t1 = B (64x16)
            {M, N}    // t2 = C (16x16)
        }
    };

    _tile_loadconfig(&cfg);
    _tile_loadd(0, A, K);     // tile 0 <- A, stride = K
    _tile_loadd(1, B, N);     // tile 1 <- B, stride = N
    _tile_zero(2);            // tile 2 <- 0
    _tile_dpbusd(2, 0, 1);    // tile 2 += tile 0 x tile 1
    _tile_stored(2, C, N);    // salva tile 2 -> C

    for (int j = 0; j < N; j++)
        printf("%6d ", C[j]);
    printf("\n");

    return 0;
}