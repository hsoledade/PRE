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

// Multiplica duas matrizes A[MxK] x B[KxN] = C[MxN] com elementos int16
void amx_matmul_int16_to_int32(const int16_t* A, const int16_t* B, int32_t* C, int M, int K, int N) {
    // AMX permite no maximo 64 bytes por linha.
    // Como cada int16_t ocupa 2 bytes, K deve ser no maximo 32.
    if (K > 32) {
        printf("Erro: K deve ser <= 32 para int16_t AMX (2B * K <= 64).\n");
        return;
    }

    for (int i0 = 0; i0 < M; i0 += 16) { // O AMX funciona com tiles de no maximo 16 linhas, independentemente do tipo de dado.
        for (int j0 = 0; j0 < N; j0 += 16) {
            int mb = (M - i0 >= 16) ? 16 : M - i0;
            int nb = (N - j0 >= 16) ? 16 : N - j0;

            // Aloca buffers temporarios para os blocos de A, B e C
            int16_t* Abuf = calloc(mb * K, sizeof(int16_t));   // O calloc inicializa com zero
            int16_t* Bbuf = calloc(K * nb, sizeof(int16_t));    
            int32_t* Cbuf = calloc(mb * nb, sizeof(int32_t));   

            // Copia um bloco de A para o buffer linear Abuf
            for (int ii = 0; ii < mb; ii++)
                memcpy(&Abuf[ii * K], &A[(i0 + ii) * K], K * sizeof(int16_t));

            // Copia um bloco de B para o buffer linear Bbuf
            for (int kk = 0; kk < K; kk++)
                memcpy(&Bbuf[kk * nb], &B[kk * N + j0], nb * sizeof(int16_t));

            // Configura os tiles AMX com base nos blocos (cada tile tem seu tamanho e largura de linha)
            _tile_conf_t cfg = {0};
            cfg.palette_id = 1;                    // Paleta 1 = manualmente configuravel
            cfg.startRow[0] = mb; cfg.colsb[0] = K * 2;      // Tile 0: A -> mb linhas x K colunas (2B cada) // colsb = Numero de bytes por linha
            cfg.startRow[1] = K;  cfg.colsb[1] = nb * 2;     // Tile 1: B -> K linhas x nb colunas (2B cada)
            cfg.startRow[2] = mb; cfg.colsb[2] = nb * 4;     // Tile 2: C -> mb linhas x nb colunas (4B cada)

            _tile_loadconfig(&cfg);                

            __tile1024i t0, t1, t2;  // Pode armazenar no maximo : 32 linhas x 32 bytes = 1024 bytes
            __tile_loadd(&t0, Abuf, K * 2);        // Carrega tile t0 com bloco de A
            __tile_loadd(&t1, Bbuf, nb * 2);       // Carrega tile t1 com bloco de B
            __tile_zero(&t2);                      // Inicializa tile t2 com zeros (acumulador)

            __tile_dpwssd(&t2, t0, t1);            
            __tile_stored(Cbuf, nb * 4, t2);       // Salva o conteudo de t2 no buffer Cbuf

            _tile_release();                       

            for (int ii = 0; ii < mb; ii++)
                memcpy(&C[(i0 + ii) * N + j0], &Cbuf[ii * nb], nb * sizeof(int32_t));

            free(Abuf);
            free(Bbuf);
            free(Cbuf);
        }
    }
}

int main() {
    enable_amx_or_exit();

    int M = 24, K = 28, N = 40;

    int16_t* A = calloc(M * K, sizeof(int16_t));
    int16_t* B = calloc(K * N, sizeof(int16_t));
    int32_t* C = calloc(M * N, sizeof(int32_t));

    for (int i = 0; i < M * K; i++) A[i] = 1;
    for (int i = 0; i < K * N; i++) B[i] = 2;

    amx_matmul_int16_to_int32(A, B, C, M, K, N);

    // Imprime a primeira linha do resultado C
    for (int j = 0; j < N; j++)
        printf("%6d ", C[j]);
    printf("\\n");

    free(A); free(B); free(C);
    return 0;
}
