#define __AMX_TILE__
#include <immintrin.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/prctl.h>
void enable_amx_or_exit() {
    if (prctl(0x53, 0x1) != 0) {
        perror("Erro ao ativar AMX");
        exit(1);
    }
    else {
        puts("AMX ativado com sucesso.");
    }
}

// Multiplica A[MxK] x B[KxN] = C[MxN]
// A: uint8_t*, B: int8_t*, C: int32_t* -> vetores lineares
void amx_matmul_int8_to_int32(const uint8_t* A, const int8_t* B, int32_t* C, int M, int K, int N) {
    // AMX permite no maximo 64 bytes por linha.
    // Como cada int8_t ocupa 1 byte, K deve ser no maximo 64.
    if (K > 64) {
        printf("Erro: K deve ser <= 64 para AMX INT8.\n");
        return;
    }

    for (int i0 = 0; i0 < M; i0 += 16) {  // O AMX funciona com tiles de no maximo 16 linhas, independentemente do tipo de dado.
        for (int j0 = 0; j0 < N; j0 += 16) {
            int mb = (M - i0 >= 16) ? 16 : M - i0;
            int nb = (N - j0 >= 16) ? 16 : N - j0;

            // Buffers temporarios com base no tamanho real dos blocos
            uint8_t* Abuf = calloc(mb * K, sizeof(uint8_t)); // O calloc inicializa com zero
            int8_t* Bbuf  = calloc(K * nb, sizeof(int8_t));
            int32_t* Cbuf = calloc(mb * nb, sizeof(int32_t));

            // Copia bloco A[i0 : i0+mb][0 : K]
            for (int ii = 0; ii < mb; ii++)
                memcpy(&Abuf[ii * K], &A[(i0 + ii) * K], K);

            // Copia bloco B[0 : K][j0 : j0+nb]
            for (int kk = 0; kk < K; kk++)
                memcpy(&Bbuf[kk * nb], &B[kk * N + j0], nb);

            // Configuracao dos tiles para o bloco atual
            _tile_conf_t cfg = {0};
            cfg.palette_id = 1;

            cfg.startRow[0] = mb;        cfg.colsb[0] = K;         // Tile A: mb x K (uint8_t -> 1B) // Colsb = Numero de bytes por linha
            cfg.startRow[1] = K;         cfg.colsb[1] = nb;        // Tile B: K x nb (int8_t -> 1B)
            cfg.startRow[2] = mb;        cfg.colsb[2] = nb * 4;    // Tile C: mb x nb (int32_t -> 4B)

            _tile_loadconfig(&cfg);

            __tile1024i t0, t1, t2; // Pode armazenar no maximo : 16 linhas x 64 bytes = 1024 bytes
            __tile_loadd(&t0, Abuf, K);         // Carrega tile A
            __tile_loadd(&t1, Bbuf, nb);        // Carrega tile B
            __tile_zero(&t2);                   // Zera tile C (acumulador)

            __tile_dpbusd(&t2, t0, t1);         // Produto: A x B -> C
            __tile_stored(Cbuf, nb * 4, t2);    // Armazena resultado

            _tile_release();

            // Copia resultado Cbuf -> matriz C
            for (int ii = 0; ii < mb; ii++)
                memcpy(&C[(i0 + ii) * N + j0], &Cbuf[ii * nb], nb * sizeof(int32_t));

            free(Abuf); free(Bbuf); free(Cbuf);
        }
    }
}

int main() {
    enable_amx_or_exit();

    int M = 32, K = 64, N = 48;

    uint8_t* A  = calloc(M * K, sizeof(uint8_t));
    int8_t* B   = calloc(K * N, sizeof(int8_t));
    int32_t* C  = calloc(M * N, sizeof(int32_t));

    for (int i = 0; i < M * K; i++) A[i] = 1;
    for (int i = 0; i < K * N; i++) B[i] = 2;

    amx_matmul_int8_to_int32(A, B, C, M, K, N);

    // Exibe os 16 primeiros valores da primeira linha de C
    for (int j = 0; j < 16 && j < N; j++)
        printf("%6d ", C[j]);
    printf("\n");

    free(A); free(B); free(C);
    return 0;
}
