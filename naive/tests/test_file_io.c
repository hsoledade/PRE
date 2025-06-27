#include <stdio.h>
#include <assert.h>
#include "file_io.h"
#include "matrix_utils.h"

#define TMP_FILE "results/test_file_io_output.txt"

int main() {
    int n = 2, m = 3;

    // Step 1: Create and fill original matrix
    int** original = allocate_matrix(n, m);

    original[0][0] = 10;
    original[0][1] = 20;
    original[0][2] = 30;
    original[1][0] = 40;
    original[1][1] = 50;
    original[1][2] = 60;

    // Step 2: Save to temporary file
    write_matrix_to_file(TMP_FILE, original, n, m);

    // Step 3: Read matrix back from file
    int read_n, read_m;
    int** recovered = read_matrix_from_file(TMP_FILE, &read_n, &read_m);

    // Step 4: Validate dimensions
    assert(read_n == n);
    assert(read_m == m);

    // Step 5: Compare values element by element
    for (int i = 0; i < n; i++)
        for (int j = 0; j < m; j++)
            assert(original[i][j] == recovered[i][j]);

    // Step 6: Visual check
    printf("test_file_io: recovered matrix content:\n");
    print_matrix(recovered, read_n, read_m);

    // Step 7: Clean up
    free_matrix(original, n);
    free_matrix(recovered, read_n);

    return 0;
}
