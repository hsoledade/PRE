#include <immintrin.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#define N 32

void matmul_int8_flat_avx512_vnni(const int8_t* A, const int8_t* B, int32_t* C, int n) {
    memset(C, 0, sizeof(int32_t) * n * n);

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; j += 16) {
            __m512i c_vec = _mm512_setzero_si512();

            for (int k = 0; k < n; k += 4) {
                // bloco A[i][k..k+3] expandido para 16 colunas
                uint8_t a_packed[64];
                for (int t = 0; t < 16; ++t) {
                    a_packed[t*4 + 0] = (uint8_t)A[i * n + (k + 0)];
                    a_packed[t*4 + 1] = (uint8_t)A[i * n + (k + 1)];
                    a_packed[t*4 + 2] = (uint8_t)A[i * n + (k + 2)];
                    a_packed[t*4 + 3] = (uint8_t)A[i * n + (k + 3)];
                }
                __m512i a_vec = _mm512_loadu_si512((__m512i*)a_packed);

                // bloco transposto de B[k..k+3][j..j+15]
                int8_t b_packed[64];
                for (int row = 0; row < 4; ++row)
                    for (int col = 0; col < 16; ++col)
                        b_packed[row*16 + col] = B[(k + row) * n + (j + col)];
                __m512i b_vec = _mm512_loadu_si512((__m512i*)b_packed);

                // VNNI: A * B + C
                c_vec = _mm512_dpbusd_epi32(c_vec, a_vec, b_vec);
            }

            _mm512_storeu_si512((__m512i*)&C[i * n + j], c_vec);
        }
    }
}

int main() {
    int8_t A[N * N], B[N * N];
    int32_t C[N * N];

    // Preenche com padrões simples
    for (int i = 0; i < N * N; ++i) {
        A[i] = (int8_t)(i % 127);
        B[i] = (int8_t)((i * 2) % 127);
    }

    matmul_int8_flat_avx512_vnni(A, B, C, N);

    printf("C[0][0] = %d\n", C[0]);
    return 0;
}
