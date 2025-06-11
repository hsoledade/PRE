# Fontes
SRC_INT8 = matmul_avx2_int8_to_int32.c
SRC_INT16 = matmul_avx2_int16_to_int32.c

# Binários
BIN_INT8 = matmul_int8
BIN_INT16 = matmul_int16

# Compilador e flags
CC = gcc
CFLAGS = -O3 -Wall -mavx2

# Compilar todos
all: $(BIN_INT8) $(BIN_INT16)

$(BIN_INT8): $(SRC_INT8)
	$(CC) $(CFLAGS) -o $@ $<

$(BIN_INT16): $(SRC_INT16)
	$(CC) $(CFLAGS) -o $@ $<

# Executar os dois
run: $(BIN_INT8) $(BIN_INT16)
	@echo "Running int8 version:"
	./$(BIN_INT8)
	@echo "\nRunning int8 version:"
	./$(BIN_INT16)

# Limpar os binários
clean:
	rm -f $(BIN_INT8) $(BIN_INT16)
