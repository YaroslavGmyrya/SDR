#include <stdint.h>
#include <stdlib.h>

int8_t* QPSK_demodulator(int* symbols, int symbols_size);
int8_t* BPSK_demodulator(int16_t* symbols, int symbols_size);
int16_t* BPSK_quantizater(double* symbols, int symbols_size);;
