#define __AMX_TILE__
#include <immintrin.h>
#include <stdint.h>
#include <stdio.h>

int main() {
    // Dados: A (2x4) uint8, B (4x2) int8
    uint8_t A[8] = {
        1, 2, 3, 4,
        5, 6, 7, 8
    };

    int8_t B[8] = {
        1, 0,
        0, 1,
        1, 0,
        0, 1
    };

    int32_t C[4] = {0};

    // Configuracao do tile (ate 64 bytes por linha)
    _tile_conf_t cfg = {0};
    cfg.palette_id = 1;
    cfg.startRow[0] = 2; cfg.colsb[0] = 4;   // A: 2x4 uint8_t
    cfg.startRow[1] = 4; cfg.colsb[1] = 2;   // B: 4x2 int8_t
    cfg.startRow[2] = 2; cfg.colsb[2] = 8;   // C: 2x2 int32_t (2 cols x 4 bytes = 8)

    _tile_loadconfig(&cfg);

    __tile1024i t0, t1, t2;
    __tile_loadd(&t0, A, 4);
    __tile_loadd(&t1, B, 2);
    __tile_zero(&t2);
    __tile_dpbusd(&t2, t0, t1);
    __tile_stored(C, 8, t2);
    _tile_release();

    printf("Resultado (C = A x B):\n");
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            printf("%6d ", C[i * 2 + j]);
        }
        printf("\n");
    }

    return 0;
}
