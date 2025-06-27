#!/bin/bash

# =============================
# Script to compile and run all unit tests
# =============================

echo ""
echo "Running all unit tests in tests/..."
echo ""

# Color codes
GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m' # No Color

TOTAL=0
PASSED=0

run_test() {
    TEST_NAME=$1
    SRC=$2
    COMPILE_CMD=$3

    eval "$COMPILE_CMD -o $TEST_NAME.out 2> /dev/null"

    if [ $? -ne 0 ]; then
        echo -e "${RED}$TEST_NAME: compilation failed${NC}"
        return
    fi

    ./$TEST_NAME.out > tmp_test_output.txt
    STATUS=$?

    echo ""
    cat tmp_test_output.txt

    if [ $STATUS -eq 0 ]; then
        echo -e "${GREEN}$TEST_NAME: passed${NC}"
        ((PASSED++))
    else
        echo -e "${RED}$TEST_NAME: failed${NC}"
    fi

    ((TOTAL++))
    rm -f $TEST_NAME.out tmp_test_output.txt
}

# ==== List of tests ====
run_test "test_matrix_utils" "tests/test_matrix_utils.c" \
"gcc -Iinclude tests/test_matrix_utils.c src/matrix_utils.c"

run_test "test_matrix_utils_gmp" "tests/test_matrix_utils_gmp.c" \
"gcc -Iinclude tests/test_matrix_utils_gmp.c src/matrix_utils_gmp.c -lgmp"

run_test "test_matrix_utils_int8" "tests/test_matrix_utils_int8.c" \
"gcc -Iinclude tests/test_matrix_utils_int8.c src/matrix_utils_int8.c"

run_test "test_matrix_utils_int16" "tests/test_matrix_utils_int16.c" \
"gcc -Iinclude tests/test_matrix_utils_int16.c src/matrix_utils_int16.c"

#############

run_test "test_file_io" "tests/test_file_io.c" \
"gcc -Iinclude tests/test_file_io.c src/file_io.c src/matrix_utils.c"

run_test "test_file_io_gmp" "tests/test_file_io_gmp.c" \
"gcc -Iinclude tests/test_file_io_gmp.c src/file_io_gmp.c src/matrix_utils_gmp.c -lgmp"

run_test "test_file_io_int8" "tests/test_file_io_int8.c" \
"gcc -Iinclude tests/test_file_io_int8.c src/file_io_int8.c src/matrix_utils_int8.c"

run_test "test_file_io_int16" "tests/test_file_io_int16.c" \
"gcc -Iinclude tests/test_file_io_int16.c src/file_io_int16.c src/matrix_utils_int16.c"

#############

run_test "test_rns_conversion" "tests/test_rns_conversion.c" \
"gcc -Iinclude tests/test_rns_conversion.c src/rns_conversion_int.c src/matrix_utils.c"

run_test "test_rns_conversion_gmp" "tests/test_rns_conversion_gmp.c" \
"gcc -Iinclude tests/test_rns_conversion_gmp.c src/rns_conversion_gmp.c src/matrix_utils_gmp.c -lgmp"

run_test "test_rns_conversion_int8" "tests/test_rns_conversion_int8.c" \
"gcc -Iinclude tests/test_rns_conversion_int8.c src/rns_conversion_int8.c src/matrix_utils_int8.c"

run_test "test_rns_conversion_int16" "tests/test_rns_conversion_int16.c" \
"gcc -Iinclude tests/test_rns_conversion_int16.c src/rns_conversion_int16.c src/matrix_utils_int16.c"

#############

run_test "test_matrix_rns_mul_int8" "tests/test_matrix_rns_mul_int8.c" \
"gcc -Iinclude tests/test_matrix_rns_mul_int8.c src/matrix_rns_mul_int8.c src/rns_conversion_int8.c src/matrix_utils_int8.c"



# ==== Summary ====
echo ""
echo "Summary: $PASSED out of $TOTAL tests passed."
echo ""

if [ $PASSED -eq $TOTAL ]; then
    exit 0
else
    exit 1
fi
