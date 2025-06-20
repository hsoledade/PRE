# Fontes
SRC_INT8 = matmul_amx_int8_to_int32.c
SRC_INT16 = matmul_amx_int16_to_int32.c

# Binarios
BIN_INT8 = matmul_int8
BIN_INT16 = matmul_int16

# Compilador e flags
CC = gcc
CFLAGS = -O3 -Wall -mavx512f -mamx-int8 -mamx-int16 -mamx-tile

# Compilar todos
all: $(BIN_INT8) $(BIN_INT16)

$(BIN_INT8): $(SRC_INT8)
	$(CC) $(CFLAGS) -o $@ $<

$(BIN_INT16): $(SRC_INT16)
	$(CC) $(CFLAGS) -o $@ $<

# Executar os binarios
run: $(BIN_INT8) $(BIN_INT16)
	@echo "Running INT8 version:"
	./$(BIN_INT8)
	@echo ""
	@echo "Running INT16 version:"
	./$(BIN_INT16)

# Limpar os binarios
clean:
	rm -f $(BIN_INT8) $(BIN_INT16)

