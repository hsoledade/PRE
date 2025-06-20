#include <stdio.h>
#include <immintrin.h>
#include <stdint.h>

/* OBSERVACAO IMPORTANTE : Internamente ele eh assim a estrutura

typedef struct __tile_config {
  unsigned char palette_id;
  unsigned char startRow[16];
  unsigned char colsb[16];
  unsigned char reserved[48];
} __tile_conf_t;
 
ESSA DEFINICAO ESTA NA PROPRIA BLIBLIOTECA DO #include <immintrin.h>
*/

// Tile = Bloco de matriz processado de forma otimizada (normalmente na memoria interna do processador).

int main() {
    // Configuracao dos tiles: 16x16 int8 -> 16x64 bytes
    _tile_conf_t cfg = {0};
    cfg.palette_id = 1; // Paleta padrao , nessa paleta eu posso escolher o tipo, se for 1 significa que eu posso editar depois as configuracoes dele 
    cfg.startRow[0] = 16; // Ta inicializando 16 linhas para o tile 0
    cfg.colsb[0] = 16; //16 bytes por linha (ex: 16 int8)
    // A: 16x16 int8
    cfg.startRow[1] = 16; cfg.colsb[1] = 16;  // B: 16x16 int8
    cfg.startRow[2] = 16; cfg.colsb[2] = 64;  // C: 16x16 int32
    _tile_loadconfig(&cfg); // Carrega a configuracao no hardware AMX

    // Alocacao linear
    int8_t A[256], B[256];
    int32_t C[256];

    for (int i = 0; i < 256; i++) {
        A[i] = 1;
        B[i] = 2;
        C[i] = 0;
    }

    // Registradores AMX
    __tile1024i t0, t1, t2;

    // Carrega tiles com __tile_loadd (ponteiro + stride)
    __tile_loadd(&t0, A, 16);   // Tile 0 <- A
    __tile_loadd(&t1, B, 16);   // Tile 1 <- B
    __tile_zero(&t2);           // Zera Tile 2

    __tile_dpbusd(&t2, t0, t1);
    __tile_stored(C, 16, t2);

    printf("C[0] = %d\n", C[0]);  // Mostra primeiro valor

    _tile_release(); 

    return 0;
}
