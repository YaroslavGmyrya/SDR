#define SAMPLES_ON_BIT 10

int* BPSK_modulation(uint8_t* bits, int bits_count, int* symbols_count);
int* QPSK_modulation(uint8_t* bits, int bits_count, int* symbols_count);
int* upsampling(int* symbols, int symbols_count, int L, int* out_size);