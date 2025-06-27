#include <immintrin.h>  // for AVX2 intrinsics
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// A_(M x K) times B_(K x N) = C_(M x N) , Optimized using SIMD AVX2 instructions, without external libraries
void matmul_int8_avx512(const int8_t* A, const int8_t* B, int32_t* C, int M, int N, int K) {
    memset(C, 0, sizeof(int32_t) * M * N);

    for (int i = 0; i < M; ++i) {
        for (int j = 0; j < N; j += 16) { 
            

            /*
            __m256i acc0 = _mm256_setzero_si256();
            __m256i acc1 = _mm256_setzero_si256();

            for (int k = 0; k < K; ++k) {
                // Broadcast A[i][k] as 16 int16_t values
                __m256i a_val = _mm256_set1_epi16((int16_t)A[i * K + k]);

                // Load B[k][j ... j+15] and zero-pad if necessary
                int8_t temp_b[16] = {0};
                int count = (j + 16 <= N) ? 16 : N - j;
                memcpy(temp_b, &B[k * N + j], count);

                // Convert 8-bit integers to 16-bit
                __m128i b_raw = _mm_loadu_si128((__m128i*)temp_b);
                __m256i b_vals = _mm256_cvtepi8_epi16(b_raw);

                // Multiply element-wise
                __m256i prod = _mm256_mullo_epi16(a_val, b_vals);

                // Extend to 32-bit and accumulate
                __m256i low = _mm256_cvtepi16_epi32(_mm256_castsi256_si128(prod));
                __m256i high = _mm256_cvtepi16_epi32(_mm256_extracti128_si256(prod, 1));

                acc0 = _mm256_add_epi32(acc0, low);
                acc1 = _mm256_add_epi32(acc1, high);
                
            }

            // Store result in C
            int count = (j + 16 <= N) ? 16 : N - j;
            if (count >= 8) _mm256_storeu_si256((__m256i*)&C[i * N + j], acc0);
            if (count > 8) _mm256_storeu_si256((__m256i*)&C[i * N + j + 8], acc1);
            else {
                int32_t temp[8];
                _mm256_storeu_si256((__m256i*)temp, acc1);
                for (int t = 0; t < count - 8; ++t)
                    C[i * N + j + 8 + t] = temp[t];
            }
            */
        }
    }
}

int main() {
    int M = 32, K = 64, N = 48;

    int8_t* A = malloc(sizeof(int8_t) * M * K);
    int8_t* B = malloc(sizeof(int8_t) * K * N);
    int32_t* C = malloc(sizeof(int32_t) * M * N);

    for (int i = 0; i < M * K; ++i) A[i] = (int8_t)(i % 127);
    for (int i = 0; i < K * N; ++i) B[i] = (int8_t)((i * 2) % 127);

    matmul_int8_avx2_nolib(A, B, C, M, N, K);

    // Print one result to verify correctness
    printf("C[0] = %d\n", C[0]);

    free(A); free(B); free(C);
    return 0;
}