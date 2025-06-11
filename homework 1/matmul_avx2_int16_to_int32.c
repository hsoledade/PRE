#include <immintrin.h>  // for AVX2 intrinsics
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// A_(M x K) times B_(K x N) = C_(M x N) , Optimized for int16_t input and int32_t output using SIMD AVX2
void matmul_int16_avx2_nolib(const int16_t* A, const int16_t* B, int32_t* C, int M, int N, int K) {
    memset(C, 0, sizeof(int32_t) * M * N);

    for (int i = 0; i < M; ++i) {
        for (int j = 0; j < N; j += 8) {  // Process 8 columns at a time (AVX2 256-bit = 8 x int32)
            __m256i acc = _mm256_setzero_si256();

            for (int k = 0; k < K; ++k) {
                // Broadcast A[i][k] to 8 int32_t slots
                __m256i a_val = _mm256_set1_epi32((int32_t)A[i * K + k]);

                // Load B[k][j..j+7] as int16_t, zero-padded if needed
                int16_t temp_b[8] = {0};
                int count = (j + 8 <= N) ? 8 : N - j;
                memcpy(temp_b, &B[k * N + j], count * sizeof(int16_t));

                // Convert to int32_t
                __m128i b_raw = _mm_loadu_si128((__m128i*)temp_b);
                __m256i b_vals = _mm256_cvtepi16_epi32(b_raw);

                // Multiply and accumulate
                __m256i prod = _mm256_mullo_epi32(a_val, b_vals);
                acc = _mm256_add_epi32(acc, prod);
            }

            // Store result
            if (j + 8 <= N) {
                _mm256_storeu_si256((__m256i*)&C[i * N + j], acc);
            } else {
                int32_t temp[8];
                _mm256_storeu_si256((__m256i*)temp, acc);
                for (int t = 0; t < N - j; ++t)
                    C[i * N + j + t] = temp[t];
            }
        }
    }
}

int main() {
    int M = 32, K = 64, N = 48;

    int16_t* A = malloc(sizeof(int16_t) * M * K);
    int16_t* B = malloc(sizeof(int16_t) * K * N);
    int32_t* C = malloc(sizeof(int32_t) * M * N);

    for (int i = 0; i < M * K; ++i) A[i] = (int16_t)(i % 32767);
    for (int i = 0; i < K * N; ++i) B[i] = (int16_t)((i * 2) % 32767);

    matmul_int16_avx2_nolib(A, B, C, M, N, K);

    // Print one result to verify correctness
    printf("C[0] = %d\n", C[0]);

    free(A); free(B); free(C);
    return 0;
}
