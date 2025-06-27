#define AMX_EXAMPLE_MAIN
#include "amx_matrix.h"
#include "matrix_utils.h"
#include <time.h>
#include <sys/time.h>

int main() {

    //////////////////////////
    // INITIALIZE INTEL AMX //
    //////////////////////////
    
    int result = amx_init();
    if (result != AMX_SUCCESS) {
        printf("AMX initialization failed\n");
        return -1;
    }
    printf("AMX initialized successfully\n\n");
    
    
    //////////////////////////////////////////////////////////////////
    // GENERATE RANDOM i8 AND u8 MATRICES WITH DIFFERENT DIMENSIONS //
    /////////////////////////////////////////////////////////////////

    generate_random_i8_matrices(4,4);
    generate_random_i8_matrices(8,8);
    generate_random_i8_matrices(16,16);
    generate_random_i8_matrices(32,32);
    generate_random_i8_matrices(64,64);
    generate_random_i8_matrices(128,128);
    generate_random_i8_matrices(256,256);
    generate_random_i8_matrices(512,512);
    generate_random_i8_matrices(1024,1024);

    generate_random_u8_matrices(4,4);
    generate_random_u8_matrices(8,8);
    generate_random_u8_matrices(16,16);
    generate_random_u8_matrices(32,32);
    generate_random_u8_matrices(64,64);
    generate_random_u8_matrices(128,128);
    generate_random_u8_matrices(256,256);
    generate_random_u8_matrices(512,512);
    generate_random_u8_matrices(1024,1024);

    ///////////////////////////
    // LOAD MATRIX FROM FILE // 
    ///////////////////////////

    int rows, cols;
    int8_t* matrix = load_matrix_u8_from_file("matrices/uint8/8x8/matrix_0.ssv", &rows, &cols);
    print_matrix_u8(matrix, rows, cols);

    ///////////////////////
    //     BENCHMARK     //
    ///////////////////////

    benchmark_uint8_int8_matmul_one_pair(4,4,4);
    benchmark_uint8_int8_matmul_one_pair(8,8,8);
    benchmark_uint8_int8_matmul_one_pair(16,16,16);
    benchmark_uint8_int8_matmul_one_pair(32,32,32);
    benchmark_uint8_int8_matmul_one_pair(64,64,64);
    benchmark_uint8_int8_matmul_one_pair(128,128,128);
    benchmark_uint8_int8_matmul_one_pair(256,256,256);
    benchmark_uint8_int8_matmul_one_pair(512,512,512);
    benchmark_uint8_int8_matmul_one_pair(1024,1024,1024);
    
    


    /*
    benchmark_int8_uint8_all_pairs(8,8,8);
    benchmark_int8_uint8_all_pairs(16,16,16);
    benchmark_int8_uint8_all_pairs(32,32,32);
    benchmark_int8_uint8_all_pairs(64,64,64);
    benchmark_int8_uint8_all_pairs(128,128,128);
    benchmark_int8_uint8_all_pairs(256,256,256);
    benchmark_int8_uint8_all_pairs(512,512,512);
    benchmark_int8_uint8_all_pairs(1024,1024,1024);
    */

    ///////////////////////
    // CODE EXAMPLE TEST //
    ///////////////////////

    /*
    //int M1 = 128, K1 = 256, N1 = 64;
    int M1 = 512, K1 = 1024, N1 = 256;
    //int M1 = 16, K1 = 64, N1 = 16;
    //int M1 = 13, K1 = 24, N1 = 12;
    
    printf("Allocating matrices... ");
    fflush(stdout);
    
    uint8_t* A1 = malloc(M1 * K1 * sizeof(uint8_t));
    int8_t* B1 = malloc(K1 * N1 * sizeof(int8_t));
    int32_t* C1 = malloc(M1 * N1 * sizeof(int32_t));
    
    if (!A1 || !B1 || !C1) {
        printf("Allocation error\n");
        return -1;
    }
    printf("OK\n");
    
    printf("Filling data... ");
    fflush(stdout);
    fill_matrix_u8(A1, M1, K1, 1);   // Values 1–255
    fill_matrix_i8(B1, K1, N1, -32); // Values -64 to 63
    printf("OK\n");
    
    printf("Executing AMX multiplication... ");
    fflush(stdout);
    double start = get_time();
    
    result = amx_multiply_int8_to_int32(A1, B1, C1, M1, K1, N1);
    
    double end = get_time();
    printf("OK\n");
    
    if (result == AMX_SUCCESS) {
        printf("Time: %.3f ms\n", (end - start) * 1000);
        printf("FLOPS: %.2f GOPS\n", (2.0 * M1 * K1 * N1) / ((end - start) * 1e9));
        verify_result(C1, M1, N1, "FC Layer");
    } else {
        printf("Multiplication error (code %d)\n", result);
    }
    
    //save_matrix_i8_to_file(A1, M1, K1, "A1.ssv");
    //save_matrix_u8_to_file(B1, K1, N1, "B1.ssv");
    //save_matrix_i32_to_file(C1, M1, N1, "C1.ssv");

    free(A1); free(B1); free(C1);
    printf("\n");
    */
    
    return 0;
}
