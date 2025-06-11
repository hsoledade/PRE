# Compiler and flags
CC = gcc
CFLAGS = -Iinclude -Wall
LDFLAGS_GMP = -lgmp

# File structure
SRC_DIR = src
INCLUDE_DIR = include
DATA_DIR = data
RESULTS_DIR = results

# Executables
TEST_INT = test_int
TEST_GMP = test_gmp

# Source files
INT_SRCS = main_int.c $(SRC_DIR)/file_io.c $(SRC_DIR)/matrix_utils.c
GMP_SRCS = main_gmp.c $(SRC_DIR)/file_io_gmp.c $(SRC_DIR)/matrix_utils_gmp.c

# Build targets
build_int:
	$(CC) $(CFLAGS) $(INT_SRCS) -o $(TEST_INT)

build_gmp:
	$(CC) $(CFLAGS) $(GMP_SRCS) -o $(TEST_GMP) $(LDFLAGS_GMP)

# Run targets
test_int: build_int
	@echo
	@echo "Running test_int..."
	@./$(TEST_INT)
	@echo

test_gmp: build_gmp
	@echo
	@echo "Running test_gmp..."
	@./$(TEST_GMP)
	@echo

# Run both tests
all-tests: test_int test_gmp

# Clean everything
clean:
	rm -f $(TEST_INT) $(TEST_GMP)
	rm -f $(RESULTS_DIR)/*.txt
