#include "file_io.h"
#include "matrix_utils.h"

int main() {
    int n, m;
    int** A = read_matrix_from_file("data/small_matrix.txt", &n, &m);
    print_matrix(A, n, m);
    write_matrix_to_file("results/copy_A.txt", A, n, m);
    free_matrix(A, n);
    return 0;
}
